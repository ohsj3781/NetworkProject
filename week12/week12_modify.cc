#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SimpleMpduAggregation");

int main(int argc, char *argv[])
{
    uint32_t payloadSize = 1472; // bytes
    uint64_t simulationTime = 10; // seconds
    
    uint64_t maxAmpduSize=0;
    uint32_t nMpdu=0;

    CommandLine cmd;
    cmd.AddValue("payloadSize", "Payload size in bytes", payloadSize);
    cmd.AddValue("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.AddValue("nMpdu","Number of Mpdu",nMpdu);
    cmd.Parse(argc, argv);

    maxAmpduSize=nMpdu*(payloadSize+100);

    // 1. Create Nodes : Make 1 STA and 1 AP
    NodeContainer wifiStaNode;
    wifiStaNode.Create(1);
    NodeContainer wifiApNode;
    wifiApNode.Create(1);

    // 2. Create PHY layer (wireless channel)
    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy = YansWifiPhyHelper::Default();
    phy.SetChannel(channel.Create());

    // 3. Create MAC layer
    WifiMacHelper mac;
    Ssid ssid = Ssid("Week12Example");
    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssid),
                "ActiveProbing", BooleanValue(false),
                "BE_MaxAmpduSize", UintegerValue(maxAmpduSize));

    // 4. Create WLAN setting
    WifiHelper wifi;
    wifi.SetStandard(WIFI_PHY_STANDARD_80211n_5GHZ);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode", StringValue("HtMcs7"),
                                 "ControlMode", StringValue("HtMcs0"));

    // 5. Create NetDevices
    NetDeviceContainer staDevice;
    staDevice = wifi.Install(phy, mac, wifiStaNode);

    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssid),
                "BeaconInterval", TimeValue(MicroSeconds(102400)),
                "BeaconGeneration", BooleanValue(true),
                "BE_MaxAmpduSize", UintegerValue(maxAmpduSize));
    NetDeviceContainer apDevice;
    apDevice = wifi.Install(phy, mac, wifiApNode);

    // 6. Create Network layer
    /* Internet stack */
    InternetStackHelper stack;
    stack.Install(wifiApNode);
    stack.Install(wifiStaNode);

    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");
    Ipv4InterfaceContainer StaInterface;
    StaInterface = address.Assign(staDevice);
    Ipv4InterfaceContainer ApInterface;
    ApInterface = address.Assign(apDevice);

    // 7. Locate nodes
    /* Setting mobility model */
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0)); // STA
    positionAlloc->Add(Vector(1.0, 0.0, 0.0)); // AP
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNode);
    mobility.Install(wifiStaNode);

    // 8. Install applications
    PacketSinkHelper sinkHelper("ns3::UdpSocketFactory",
                                InetSocketAddress(Ipv4Address::GetAny(), 9));
    ApplicationContainer sinkApp = sinkHelper.Install(wifiApNode);
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(simulationTime + 1));
    Ptr<PacketSink> sink = StaticCast<PacketSink>(sinkApp.Get(0));

    OnOffHelper server("ns3::UdpSocketFactory",
                       InetSocketAddress(ApInterface.GetAddress(0), 9));
    server.SetAttribute("PacketSize", UintegerValue(payloadSize));
    server.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    server.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    server.SetAttribute("DataRate", DataRateValue(DataRate("100Mbps")));
    ApplicationContainer serverApp = server.Install(wifiStaNode);
    serverApp.Start(Seconds(1.0));
    serverApp.Stop(Seconds(simulationTime + 1));

    // Run simulation
    Simulator::Stop(Seconds(simulationTime + 1));
    Simulator::Run();
    Simulator::Destroy();

    // Measure throughput
    double averageThroughput = ((sink->GetTotalRx() * 8) / (1e6 * simulationTime));
    std::cout<<"\nnMpdu = "<<nMpdu<<" ";
    std::cout << " Average throughput: " << averageThroughput << " Mbit/s" << std::endl;

    return 0;
}

