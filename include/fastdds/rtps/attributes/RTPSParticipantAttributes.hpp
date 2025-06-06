// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file RTPSParticipantAttributes.hpp
 */

#ifndef FASTDDS_RTPS_ATTRIBUTES__RTPSPARTICIPANTATTRIBUTES_HPP
#define FASTDDS_RTPS_ATTRIBUTES__RTPSPARTICIPANTATTRIBUTES_HPP

#include <memory>
#include <sstream>

#include <fastcdr/cdr/fixed_size_string.hpp>

#include <fastdds/rtps/attributes/BuiltinTransports.hpp>
#include <fastdds/rtps/attributes/ExternalLocators.hpp>
#include <fastdds/rtps/attributes/PropertyPolicy.hpp>
#include <fastdds/rtps/attributes/ResourceManagement.hpp>
#include <fastdds/rtps/attributes/RTPSParticipantAllocationAttributes.hpp>
#include <fastdds/rtps/attributes/ThreadSettings.hpp>
#include <fastdds/rtps/common/Locator.hpp>
#include <fastdds/rtps/common/PortParameters.hpp>
#include <fastdds/rtps/common/Time_t.hpp>
#include <fastdds/rtps/common/Types.hpp>
#include <fastdds/rtps/flowcontrol/FlowControllerDescriptor.hpp>
#include <fastdds/rtps/transport/network/NetmaskFilterKind.hpp>
#include <fastdds/rtps/transport/TransportInterface.hpp>
#include <fastdds/fastdds_dll.hpp>

namespace eprosima {
namespace fastdds {
namespace rtps {

/**
 * Struct to define participant types to set participant type parameter property
 */
struct ParticipantType
{
    static constexpr const char* SIMPLE = "SIMPLE";
    static constexpr const char* SERVER = "SERVER";
    static constexpr const char* CLIENT = "CLIENT";
    static constexpr const char* SUPER_CLIENT = "SUPER_CLIENT";
    static constexpr const char* BACKUP = "BACKUP";
    static constexpr const char* NONE = "NONE";
    static constexpr const char* EXTERNAL = "EXTERNAL";
    static constexpr const char* UNKNOWN = "UNKNOWN";
};

}  // namespace rtps
}  // namespace fastdds

namespace fastdds {
namespace rtps {

//! PDP subclass choice
enum class DiscoveryProtocol
{
    NONE,
    /*!<
        NO discovery whatsoever would be used.
        Publisher and Subscriber defined with the same topic name would NOT be linked.
        All matching must be done manually through the addReaderLocator, addReaderProxy, addWriterProxy methods.
     */
    SIMPLE,
    /*!<
        Discovery works according to 'The Real-time Publish-Subscribe Protocol(RTPS) DDS
        Interoperability Wire Protocol Specification'
     */
    EXTERNAL,
    /*!<
        A user defined PDP subclass object must be provided in the attributes that deals with the discovery.
        Framework is not responsible of this object lifetime.
     */
    CLIENT, /*!< The participant will behave as a client concerning discovery operation.
                 Server locators should be specified as attributes. */
    SERVER, /*!< The participant will behave as a server concerning discovery operation.
                 Discovery operation is volatile (discovery handshake must take place if shutdown). */
    BACKUP,  /*!< The participant will behave as a server concerning discovery operation.
                 Discovery operation persist on a file (discovery handshake wouldn't repeat if shutdown). */
    SUPER_CLIENT  /*!< The participant will behave as a client concerning all internal behaviour.
                     Remote servers will treat it as a server and will share every discovery information. */

};

inline std::ostream& operator <<(
        std::ostream& output,
        const DiscoveryProtocol& discovery_protocol)
{
    switch (discovery_protocol)
    {
        case DiscoveryProtocol::NONE:
            output << fastdds::rtps::ParticipantType::NONE;
            break;
        case DiscoveryProtocol::SIMPLE:
            output << fastdds::rtps::ParticipantType::SIMPLE;
            break;
        case DiscoveryProtocol::EXTERNAL:
            output << fastdds::rtps::ParticipantType::EXTERNAL;
            break;
        case DiscoveryProtocol::CLIENT:
            output << fastdds::rtps::ParticipantType::CLIENT;
            break;
        case DiscoveryProtocol::SUPER_CLIENT:
            output << fastdds::rtps::ParticipantType::SUPER_CLIENT;
            break;
        case DiscoveryProtocol::SERVER:
            output << fastdds::rtps::ParticipantType::SERVER;
            break;
        case DiscoveryProtocol::BACKUP:
            output << fastdds::rtps::ParticipantType::BACKUP;
            break;
        default:
            output << fastdds::rtps::ParticipantType::UNKNOWN;
    }
    return output;
}

/**
 * Returns the guidPrefix associated to the given server id
 * @param [in] id of the default server whose guidPrefix we want to retrieve
 * @param [out] guid reference to the guidPrefix to modify
 * @return true if the server guid can be delivered
 */
FASTDDS_EXPORTED_API bool get_server_client_default_guidPrefix(
        int id,
        fastdds::rtps::GuidPrefix_t& guid);

// Port used if the ros environment variable doesn't specify one
constexpr uint16_t DEFAULT_ROS2_SERVER_PORT = 11811;
// Port used by default for tcp transport
constexpr uint16_t DEFAULT_TCP_SERVER_PORT = 42100;

//! Filtering flags when discovering participants
FASTDDS_TODO_BEFORE(4, 0, "Change it to uint8_t (implies also changing [de]serializations)");
enum ParticipantFilteringFlags : uint32_t
{
    NO_FILTER = 0,
    FILTER_DIFFERENT_HOST = 0x1,
    FILTER_DIFFERENT_PROCESS = 0x2,
    FILTER_SAME_PROCESS = 0x4
};

#define BUILTIN_DATA_MAX_SIZE 512

//! PDP factory for EXTERNAL type
class PDP;
class BuiltinProtocols;

typedef struct PDPFactory
{
    // Pointer to the PDP creator
    PDP* (*CreatePDPInstance)(BuiltinProtocols*);
    // Pointer to the PDP destructor
    void (* ReleasePDPInstance)(
            PDP*);

    bool operator ==(
            const struct PDPFactory& e) const
    {
        return (CreatePDPInstance == e.CreatePDPInstance)
               && (ReleasePDPInstance == e.ReleasePDPInstance);
    }

} PDPFactory;

/**
 * Class SimpleEDPAttributes, to define the attributes of the Simple Endpoint Discovery Protocol.
 * @ingroup RTPS_ATTRIBUTES_MODULE
 */
class SimpleEDPAttributes
{
public:

    //!Default value true.
    bool use_PublicationWriterANDSubscriptionReader;

    //!Default value true.
    bool use_PublicationReaderANDSubscriptionWriter;

#if HAVE_SECURITY
    bool enable_builtin_secure_publications_writer_and_subscriptions_reader;

    bool enable_builtin_secure_subscriptions_writer_and_publications_reader;
#endif // if HAVE_SECURITY

    SimpleEDPAttributes()
        : use_PublicationWriterANDSubscriptionReader(true)
        , use_PublicationReaderANDSubscriptionWriter(true)
#if HAVE_SECURITY
        , enable_builtin_secure_publications_writer_and_subscriptions_reader(true)
        , enable_builtin_secure_subscriptions_writer_and_publications_reader(true)
#endif // if HAVE_SECURITY
    {
    }

    bool operator ==(
            const SimpleEDPAttributes& b) const
    {
        return (this->use_PublicationWriterANDSubscriptionReader == b.use_PublicationWriterANDSubscriptionReader) &&
#if HAVE_SECURITY
               (this->enable_builtin_secure_publications_writer_and_subscriptions_reader ==
               b.enable_builtin_secure_publications_writer_and_subscriptions_reader) &&
               (this->enable_builtin_secure_subscriptions_writer_and_publications_reader ==
               b.enable_builtin_secure_subscriptions_writer_and_publications_reader) &&
#endif // if HAVE_SECURITY
               (this->use_PublicationReaderANDSubscriptionWriter == b.use_PublicationReaderANDSubscriptionWriter);
    }

};

/**
 * Struct InitialAnnouncementConfig defines the behavior of the RTPSParticipant initial announcements.
 * @ingroup RTPS_ATTRIBUTES_MODULE
 */
struct InitialAnnouncementConfig
{
    /// Number of initial announcements with specific period (default 5)
    uint32_t count = 5u;

    /// Specific period for initial announcements (default 100ms)
    dds::Duration_t period = { 0, 100000000u };

    bool operator ==(
            const InitialAnnouncementConfig& b) const
    {
        return (count == b.count) && (period == b.period);
    }

};

/**
 * Class DiscoverySettings, to define the attributes of the several discovery protocols available
 * @ingroup RTPS_ATTRIBUTES_MODULE
 */

class DiscoverySettings
{
public:

    //! Chosen discovery protocol
    DiscoveryProtocol discoveryProtocol = DiscoveryProtocol::SIMPLE;

    /**
     * If set to true, SimpleEDP would be used.
     */
    bool use_SIMPLE_EndpointDiscoveryProtocol = true;

    /**
     * If set to true, StaticEDP based on an XML file would be implemented.
     * The XML filename must be provided.
     */
    bool use_STATIC_EndpointDiscoveryProtocol = false;

    /**
     * Lease Duration of the RTPSParticipant,
     * indicating how much time remote RTPSParticipants should consider this RTPSParticipant alive.
     */
    dds::Duration_t leaseDuration = { 20, 0 };

    /**
     * The period for the RTPSParticipant to send its Discovery Message to all other discovered RTPSParticipants
     * as well as to all Multicast ports.
     */
    dds::Duration_t leaseDuration_announcementperiod = { 3, 0 };

    //!Initial announcements configuration
    InitialAnnouncementConfig initial_announcements;

    //!Attributes of the SimpleEDP protocol
    SimpleEDPAttributes m_simpleEDP;

    //! function that returns a PDP object (only if EXTERNAL selected)
    PDPFactory m_PDPfactory{};
    /**
     * The period for the RTPSParticipant to:
     *  send its Discovery Message to its servers
     *  check for EDP endpoints matching
     */
    dds::Duration_t discoveryServer_client_syncperiod = { 0, 450 * 1000000}; // 450 milliseconds

    //! Discovery Server initial connections, needed if `discoveryProtocol` = CLIENT | SUPER_CLIENT | SERVER | BACKUP
    eprosima::fastdds::rtps::LocatorList m_DiscoveryServers;

    //! Filtering participants out depending on location
    ParticipantFilteringFlags ignoreParticipantFlags = ParticipantFilteringFlags::NO_FILTER;

    DiscoverySettings() = default;

    bool operator ==(
            const DiscoverySettings& b) const
    {
        return (this->discoveryProtocol == b.discoveryProtocol) &&
               (this->use_SIMPLE_EndpointDiscoveryProtocol == b.use_SIMPLE_EndpointDiscoveryProtocol) &&
               (this->use_STATIC_EndpointDiscoveryProtocol == b.use_STATIC_EndpointDiscoveryProtocol) &&
               (this->discoveryServer_client_syncperiod == b.discoveryServer_client_syncperiod) &&
               (this->m_PDPfactory == b.m_PDPfactory) &&
               (this->leaseDuration == b.leaseDuration) &&
               (this->leaseDuration_announcementperiod == b.leaseDuration_announcementperiod) &&
               (this->initial_announcements == b.initial_announcements) &&
               (this->m_simpleEDP == b.m_simpleEDP) &&
               (this->static_edp_xml_config_ == b.static_edp_xml_config_) &&
               (this->m_DiscoveryServers == b.m_DiscoveryServers) &&
               (this->ignoreParticipantFlags == b.ignoreParticipantFlags);
    }

    /**
     * Set the static endpoint XML configuration.
     * @param str URI specifying the static endpoint XML configuration.
     * The string could contain a filename (file://) or the XML content directly (data://).
     */
    void static_edp_xml_config(
            const char* str)
    {
        static_edp_xml_config_ = str;
    }

    /**
     * Get the static endpoint XML configuration.
     * @return URI specifying the static endpoint XML configuration.
     * The string could contain a filename (file://) or the XML content directly (data://).
     */
    const char* static_edp_xml_config() const
    {
        return static_edp_xml_config_.c_str();
    }

private:

    //! URI specifying the static EDP XML configuration, only necessary if use_STATIC_EndpointDiscoveryProtocol=true
    //! This string could contain a filename or the XML content directly.
    std::string static_edp_xml_config_ = "";
};

/**
 * Class BuiltinAttributes, to define the behavior of the RTPSParticipant builtin protocols.
 * @ingroup RTPS_ATTRIBUTES_MODULE
 */
class BuiltinAttributes
{
public:

    //! Discovery protocol related attributes
    DiscoverySettings discovery_config;

    //! Indicates to use the WriterLiveliness protocol.
    bool use_WriterLivelinessProtocol = true;

    //! Network Configuration
    NetworkConfigSet_t network_configuration = 0;

    //! Metatraffic Unicast Locator List
    LocatorList_t metatrafficUnicastLocatorList;

    //! Metatraffic Multicast Locator List.
    LocatorList_t metatrafficMulticastLocatorList;

    //! The collection of external locators to use for communication on metatraffic topics.
    fastdds::rtps::ExternalLocators metatraffic_external_unicast_locators;

    //! Initial peers.
    LocatorList_t initialPeersList;

    //! Memory policy for builtin readers
    MemoryManagementPolicy_t readerHistoryMemoryPolicy =
            MemoryManagementPolicy_t::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;

    //! Maximum payload size for builtin readers
    uint32_t readerPayloadSize = BUILTIN_DATA_MAX_SIZE;

    //! Memory policy for builtin writers
    MemoryManagementPolicy_t writerHistoryMemoryPolicy =
            MemoryManagementPolicy_t::PREALLOCATED_WITH_REALLOC_MEMORY_MODE;

    //! Maximum payload size for builtin writers
    uint32_t writerPayloadSize = BUILTIN_DATA_MAX_SIZE;

    //! Mutation tries if the port is being used.
    uint32_t mutation_tries = 100u;

    //! Set to true to avoid multicast traffic on builtin endpoints
    bool avoid_builtin_multicast = true;

    //! Flow controller name to use for the builtin writers
    std::string flow_controller_name = "";

    BuiltinAttributes() = default;

    virtual ~BuiltinAttributes() = default;

    bool operator ==(
            const BuiltinAttributes& b) const
    {
        return (this->discovery_config == b.discovery_config) &&
               (this->use_WriterLivelinessProtocol == b.use_WriterLivelinessProtocol) &&
               (this->network_configuration == b.network_configuration) &&
               (this->metatrafficUnicastLocatorList == b.metatrafficUnicastLocatorList) &&
               (this->metatrafficMulticastLocatorList == b.metatrafficMulticastLocatorList) &&
               (this->metatraffic_external_unicast_locators == b.metatraffic_external_unicast_locators) &&
               (this->initialPeersList == b.initialPeersList) &&
               (this->readerHistoryMemoryPolicy == b.readerHistoryMemoryPolicy) &&
               (this->readerPayloadSize == b.readerPayloadSize) &&
               (this->writerHistoryMemoryPolicy == b.writerHistoryMemoryPolicy) &&
               (this->writerPayloadSize == b.writerPayloadSize) &&
               (this->mutation_tries == b.mutation_tries) &&
               (this->flow_controller_name == b.flow_controller_name) &&
               (this->avoid_builtin_multicast == b.avoid_builtin_multicast);
    }

};

/**
 * Class RTPSParticipantAttributes used to define different aspects of a RTPSParticipant.
 * @ingroup RTPS_ATTRIBUTES_MODULE
 */
class RTPSParticipantAttributes
{
    using FlowControllerDescriptorList = std::vector<std::shared_ptr<fastdds::rtps::FlowControllerDescriptor>>;

public:

    RTPSParticipantAttributes() = default;

    virtual ~RTPSParticipantAttributes() = default;

    bool operator ==(
            const RTPSParticipantAttributes& b) const
    {
        return (this->name == b.name) &&
               (this->defaultUnicastLocatorList == b.defaultUnicastLocatorList) &&
               (this->defaultMulticastLocatorList == b.defaultMulticastLocatorList) &&
               (this->default_external_unicast_locators == b.default_external_unicast_locators) &&
               (this->ignore_non_matching_locators == b.ignore_non_matching_locators) &&
               (this->sendSocketBufferSize == b.sendSocketBufferSize) &&
               (this->listenSocketBufferSize == b.listenSocketBufferSize) &&
               (this->netmaskFilter == b.netmaskFilter) &&
               (this->builtin == b.builtin) &&
               (this->port == b.port) &&
               (this->userData == b.userData) &&
               (this->participantID == b.participantID) &&
               (this->easy_mode_ip == b.easy_mode_ip) &&
               (this->useBuiltinTransports == b.useBuiltinTransports) &&
               (this->properties == b.properties) &&
               (this->prefix == b.prefix) &&
               (this->flow_controllers == b.flow_controllers) &&
               (this->builtin_controllers_sender_thread == b.builtin_controllers_sender_thread) &&
               (this->timed_events_thread == b.timed_events_thread) &&
#if HAVE_SECURITY
               (this->security_log_thread == b.security_log_thread) &&
#endif // if HAVE_SECURITY
               (this->discovery_server_thread == b.discovery_server_thread) &&
               (this->typelookup_service_thread == b.typelookup_service_thread) &&
               (this->builtin_transports_reception_threads == b.builtin_transports_reception_threads);

    }

    /**
     * Provides a way of easily configuring transport related configuration on certain pre-defined scenarios with
     * certain options.
     *
     * @param transports Defines the transport configuration scenario to setup.
     * @param options Defines the options to be used in the transport configuration.
     */
    FASTDDS_EXPORTED_API void setup_transports(
            fastdds::rtps::BuiltinTransports transports,
            const fastdds::rtps::BuiltinTransportsOptions& options = fastdds::rtps::BuiltinTransportsOptions());

    /**
     * Default list of Unicast Locators to be used for any Endpoint defined inside this RTPSParticipant in the case
     * that it was defined with NO UnicastLocators. At least ONE locator should be included in this list.
     */
    LocatorList_t defaultUnicastLocatorList;

    /**
     * Default list of Multicast Locators to be used for any Endpoint defined inside this RTPSParticipant in the
     * case that it was defined with NO MulticastLocators. This is usually left empty.
     */
    LocatorList_t defaultMulticastLocatorList;

    /**
     * The collection of external locators to use for communication on user created topics.
     */
    fastdds::rtps::ExternalLocators default_external_unicast_locators;

    /**
     * Whether locators that don't match with the announced locators should be kept.
     */
    bool ignore_non_matching_locators = false;

    /*!
     * @brief Send socket buffer size for the send resource. Zero value indicates to use default system buffer size.
     * Default value: 0.
     */
    uint32_t sendSocketBufferSize = 0;

    /*! Listen socket buffer for all listen resources. Zero value indicates to use default system buffer size.
     * Default value: 0.
     */
    uint32_t listenSocketBufferSize = 0;

    //! Netmask filter configuration
    fastdds::rtps::NetmaskFilterKind netmaskFilter = fastdds::rtps::NetmaskFilterKind::AUTO;

    //! Optionally allows user to define the GuidPrefix_t
    GuidPrefix_t prefix;

    FASTDDS_EXPORTED_API inline bool ReadguidPrefix(
            const char* pfx)
    {
        return bool(std::istringstream(pfx) >> prefix);
    }

    //! Builtin parameters.
    BuiltinAttributes builtin;

    //! Port Parameters
    PortParameters port;

    //! User Data of the participant
    std::vector<octet> userData;

    //! Participant ID
    int32_t participantID = -1;

    //! IP of the Host where master Server is located (EASY_MODE context)
    std::string easy_mode_ip = "";

    //! User defined transports to use alongside or in place of builtins.
    std::vector<std::shared_ptr<fastdds::rtps::TransportDescriptorInterface>> userTransports;

    //! Set as false to disable the creation of the default transports.
    bool useBuiltinTransports = true;

    //! Holds allocation limits affecting collections managed by a participant.
    RTPSParticipantAllocationAttributes allocation;

    //! Property policies
    PropertyPolicy properties;

    //! Set the name of the participant.
    inline void setName(
            const char* nam)
    {
        name = nam;
    }

    //! Get the name of the participant.
    inline const char* getName() const
    {
        return name.c_str();
    }

    //! Flow controllers.
    FlowControllerDescriptorList flow_controllers;

    //! Thread settings for the builtin flow controllers sender threads
    fastdds::rtps::ThreadSettings builtin_controllers_sender_thread;

    //! Thread settings for the timed events thread
    fastdds::rtps::ThreadSettings timed_events_thread;

    //! Thread settings for the discovery server thread
    fastdds::rtps::ThreadSettings discovery_server_thread;

    //! Thread settings for the builtin TypeLookup service requests and replies threads
    fastdds::rtps::ThreadSettings typelookup_service_thread;

    //! Thread settings for the builtin transports reception threads
    fastdds::rtps::ThreadSettings builtin_transports_reception_threads;

#if HAVE_SECURITY
    //! Thread settings for the security log thread
    fastdds::rtps::ThreadSettings security_log_thread;
#endif // if HAVE_SECURITY

    /*! Maximum message size used to avoid fragmentation, set ONLY in LARGE_DATA. If this value is
     * not zero, the network factory will allow the initialization of UDP transports with maxMessageSize
     * higher than 65500K.
     */
    uint32_t max_msg_size_no_frag = 0;

private:

    //! Name of the participant.
    fastcdr::string_255 name{"RTPSParticipant"};
};

}  // namespace rtps
}  // namespace fastdds
}  // namespace eprosima

#endif  // FASTDDS_RTPS_ATTRIBUTES__RTPSPARTICIPANTATTRIBUTES_HPP
