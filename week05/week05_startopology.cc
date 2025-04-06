#include <iostream>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/bridge-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("week5StarScriptExample");

static void
Rxtime (std::string context,Ptr<const Packet> p,const Address &a)
{
    static double bytes1, bytes2=0;
    if (context=="Flow1"){
        bytes1+=p->GetSize();
        NS_LOG_UNCOND("1\t"<<Simulator::Now().GetSeconds()
    <<"\t"<<bytes1*8/1000000/(Simulator::Now().GetSeconds()-1));
    } else if(context=="Flow2"){
        bytes2+=p->GetSize();
        NS_LOG_UNCOND("2\t"<<Simulator::Now().GetSeconds()
        <<"\t"<<bytes2*8/1000000/(Simulator::Now().GetSeconds()-3));
    }
}

int main(int argc, char* argv[]){
    
    CommandLine cmd;
    cmd.Parse(argc,argv);

    // Create nodes
    NS_LOG_INFO("Create nodes.");
    NodeContainer terminals;
    terminals.Create(4);

    NodeContainer csmaSwitch;
    csmaSwitch.Create(1);

    // Create links
    NS_LOG_INFO("Build Topology");
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate",DataRateValue(5000000));
    csma.SetChannelAttribute("Delay",TimeValue(MilliSeconds(2)));

    NetDeviceContainer terminalDevices;
    NetDeviceContainer switchDevices;

    // Connect switch node and terminal node
    for(int i=0;i<4;i++){
        NetDeviceContainer link=csma.Install(NodeContainer(terminals.Get(i),csmaSwitch));
        terminalDevices.Add(link.Get(0));
        switchDevices.Add(link.Get(1));
    }

    Ptr<Node> switchNode=csmaSwitch.Get(0);
    BridgeHelper bridge;
    bridge.Install(switchNode,switchDevices);

    // Install Internet stack an assign IP adresses
    InternetStackHelper internet;
    internet.Install (terminals);

    NS_LOG_INFO("Assign Ip Addresses.");
    Ipv4AddressHelper ipv4;
    ipv4.SetBase("10.1.1.0","255.255.255.0");
    ipv4.Assign(terminalDevices);

    // Create an OnOff application to send UDP datagrams from node 0 to node 1
    NS_LOG_INFO("Create Application");
    uint16_t port=9;

    OnOffHelper onoff("ns3::UdpSocketFactory",
        Address(InetSocketAddress(Ipv4Address("10.1.1.2"),port)));
    onoff.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    onoff.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=1.0]"));
    onoff.SetAttribute("DataRate",DataRateValue(5000000));

    // Start the application
    ApplicationContainer app1=onoff.Install(terminals.Get(0));
    app1.Start(Seconds(1.0));
    app1.Stop(Seconds(10.0));

    // Create an optional packet sink to receive these packets
    PacketSinkHelper sink("ns3::UdpSocketFactory",
        Address(InetSocketAddress(Ipv4Address::GetAny(),port)));

    ApplicationContainer sinkApp1=sink.Install(terminals.Get(1));
    sinkApp1.Start(Seconds(1.0));
    sinkApp1.Get(0)->TraceConnect("Rx","Flow1",MakeCallback(&Rxtime));

    // Create a similar flow from n3 to n0, starting at time 1.3 seconds
    onoff.SetAttribute("Remote",
        AddressValue(InetSocketAddress(Ipv4Address("10.1.1.1"),port)));
    onoff.SetAttribute("DataRate",DataRateValue(10000000));
    onoff.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=0.3]"));
    onoff.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0.7]"));

    ApplicationContainer app2=onoff.Install(terminals.Get(3));
    // Start the application
    app2.Start(Seconds(3.0));
    app2.Stop(Seconds(13.0));

    ApplicationContainer sinkApp2=sink.Install(terminals.Get(0));
    sinkApp2.Start(Seconds(3.0));
    sinkApp2.Get(0)->TraceConnect("Rx","Flow2",MakeCallback(&Rxtime));

    // Enable Pcap tracing
    csma.EnablePcapAll("week5_start",false);

    NS_LOG_INFO("Run Simulation.");
    Simulator::Stop(Seconds(15));
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");
}