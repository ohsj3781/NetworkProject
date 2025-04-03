#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include <cstdio>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("UdpEchoExample");

int main(int argc,char * argv[]){

    std::string delay="10us";
    uint32_t packet_size=1024;
    std::string bw="10Mbps";


    CommandLine cmd;
    cmd.AddValue("Delay","Line Delay",delay);
    cmd.AddValue("DataRate","BandWidth",bw);
    cmd.Parse (argc,argv);

    NodeContainer p2pNodes;
    p2pNodes.Create(2);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate",StringValue(bw));
    pointToPoint.SetChannelAttribute("Delay",StringValue(delay));

    NetDeviceContainer p2pDevices;
    p2pDevices=pointToPoint.Install(p2pNodes);

    InternetStackHelper stack;
    stack.Install(p2pNodes.Get(0));
    stack.Install(p2pNodes.Get(1));

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces=address.Assign(p2pDevices);

    // UdpEchoServerHelper echoServer(9);
    // ApplicationContainer serverApps=echoServer.Install(p2pNodes.Get(1));
    // serverApps.Start(Seconds(1.0));
    // serverApps.Stop(Seconds(10.0));

    PacketSinkHelper sinkServer("ns3::UdpSocketFactory",InetSocketAddress(Ipv4Address::GetAny(),9));
    ApplicationContainer serverApp=sinkServer.Install(p2pNodes.Get(1));

    // UdpEchoClientHelper echoClient(p2pInterfaces.GetAddress(1),9);
    // echoClient.SetAttribute("MaxPackets",UintegerValue(1024));
    // echoClient.SetAttribute("Interval",TimeValue(Seconds(1.0)));
    // echoClient.SetAttribute("PacketSize",UintegerValue(packet_size));

    OnOffHelper clientHelper("ns3::UdpSocketFactory",InetSocketAddress(Ipv4Address(p2pInterfaces.GetAddress(1)),9));
    clientHelper.SetAttribute("PacketSize",UintegerValue(packet_size));
    clientHelper.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    clientHelper.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0]"));
    clientHelper.SetAttribute("DataRate",StringValue("1Mbps"));

    ApplicationContainer clientApps=clientHelper.Install(p2pNodes.Get(0));
    clientApps.Start(Seconds(1.0));
    clientApps.Stop(Seconds(10.0));

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // pointToPoint.EnablePcapAll("2020314916-");
    pointToPoint.EnablePcap("2020314916-client",p2pDevices.Get(0));
    pointToPoint.EnablePcap("2020314916-server",p2pDevices.Get(1));

    Simulator::Run();
    Simulator::Destroy();

    std::rename("2020314916-client-0-0.pcap","2020314916-client.pcap");
    std::rename("2020314916-server-1-0.pcap","2020314916-server.pcap");

    return 0;
}