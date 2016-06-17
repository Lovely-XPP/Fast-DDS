#include "FragmentedChangePitStop.h"
#include <fastrtps/rtps/common/CacheChange.h>
#include <fastrtps/rtps/reader/RTPSReader.h>

using namespace eprosima::fastrtps::rtps;

CacheChange_t* FragmentedChangePitStop::process(CacheChange_t* incoming_change, uint32_t sampleSize, uint32_t fragmentStartingNum)
{
    CacheChange_t* returnedValue = nullptr;

    // Search CacheChange_t with the sample sequence number.
    auto range = changes_.equal_range(ChangeInPit(incoming_change));

    auto original_change_cit = range.first;

    // If there is a range, search the CacheChange_t with the same writer GUID_t.
    if(original_change_cit != changes_.end())
    {
        for(; original_change_cit != range.second; ++original_change_cit)
        {
            if(original_change_cit->getChange()->writerGUID == incoming_change->writerGUID)
                break;
        }
    }
    else
        original_change_cit = range.second;

    // If not found an existing CacheChange_t, reserve one and insert.
    if(original_change_cit == range.second)
    {
        CacheChange_t* original_change = nullptr;

        if(!parent_->reserveCache(&original_change))
            return nullptr;

        original_change->copy_not_memcpy(incoming_change);

        // The length of the serialized payload has to be sample size.
        original_change->serializedPayload.length = sampleSize;
        original_change->setFragmentSize(incoming_change->getFragmentSize());

        // Insert
        original_change_cit = changes_.insert(ChangeInPit(original_change));
    }

    bool was_updated = false;
    for (uint32_t count = (fragmentStartingNum - 1); count < (fragmentStartingNum - 1) + incoming_change->getFragmentCount(); ++count)
    {
        if(original_change_cit->getChange()->getDataFragments()->at(count) == ChangeFragmentStatus_t::NOT_PRESENT)
        {
            // All cases minus last fragment.
            if (count + 1 != original_change_cit->getChange()->getFragmentCount())
            {
                memcpy(original_change_cit->getChange()->serializedPayload.data + count * original_change_cit->getChange()->getFragmentSize(),
                        incoming_change->serializedPayload.data + (count - (fragmentStartingNum - 1)) * incoming_change->getFragmentSize(), incoming_change->getFragmentSize());
            }
            // Last fragment is a special case when copying.
            else
            {
                memcpy(original_change_cit->getChange()->serializedPayload.data + count * original_change_cit->getChange()->getFragmentSize(),
                        incoming_change->serializedPayload.data + (count - (fragmentStartingNum - 1)) * incoming_change->getFragmentSize(), original_change_cit->getChange()->serializedPayload.length - (count * original_change_cit->getChange()->getFragmentSize()));
            }

            original_change_cit->getChange()->getDataFragments()->at(count) = ChangeFragmentStatus_t::PRESENT;

            if(fragmentStartingNum == 1)
            {  
                // Update encapsulation
                original_change_cit->getChange()->serializedPayload.encapsulation = incoming_change->serializedPayload.encapsulation;
            }

            was_updated = true;
        }
    }

    // If was updated, check if it is completed.
    if(was_updated)
    {
        auto fit = original_change_cit->getChange()->getDataFragments()->begin();
        for(; fit != original_change_cit->getChange()->getDataFragments()->end(); ++fit)
        {
            if(*fit == ChangeFragmentStatus_t::NOT_PRESENT)
                break;
        }

        // If it is completed, return CacheChange_t and remove information.
        if(fit == original_change_cit->getChange()->getDataFragments()->end())
        {
            returnedValue = original_change_cit->getChange();
            changes_.erase(original_change_cit);
        }
    }

    return returnedValue;
}

CacheChange_t* FragmentedChangePitStop::find(const SequenceNumber_t& sequence_number, const GUID_t& writer_guid)
{
    CacheChange_t* returnedValue = nullptr;

    auto range = changes_.equal_range(ChangeInPit(sequence_number));

    auto cit = range.first;

    // If there is a range, search the CacheChange_t with the same writer GUID_t.
    if(cit != changes_.end())
    {
        for(; cit != range.second; ++cit)
        {
            if(cit->getChange()->writerGUID == writer_guid)
            {
                returnedValue = cit->getChange();
                break;
            }
        }
    }

    return returnedValue;
}