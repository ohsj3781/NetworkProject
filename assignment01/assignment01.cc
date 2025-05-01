#include <iostream>
#include <fstream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/bridge-module.h"
#include "ns3/csma-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-routing-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("Assignment 01");

static double g_bytesFlow1=0;
static double g_bytesFlow2=0;
static double g_bytesFlowBack=0;


static void
Rxtime (std::string context,Ptr<const Packet> p,const Address &a)
{
    // static double bytes1, bytes2, bytes3=0;
    if (context=="Flow1"){
        // bytes1+=p->GetSize();
        g_bytesFlow1+=p->GetSize();
    //     NS_LOG_UNCOND("1\t"<<Simulator::Now().GetSeconds()
    // <<"\t"<<bytes1*8/1000000/(Simulator::Now().GetSeconds()-10));
    } else if(context=="Flow2"){
        // bytes2+=p->GetSize();
        g_bytesFlow2+=p->GetSize();
        // NS_LOG_UNCOND("2\t"<<Simulator::Now().GetSeconds()
        // <<"\t"<<bytes2*8/1000000/(Simulator::Now().GetSeconds()-12));
    }
    else if(context=="FlowBack"){
        g_bytesFlowBack+=p->GetSize();
        // NS_LOG_UNCOND("3\t"<<Simulator::Now().GetSeconds()
        // <<"\t"<<bytes3*8/1000000/(Simulator::Now().GetSeconds()-4));
    }
}

static double g_lastTime=0.0;

void MeasureThroughput(){
    double now=Simulator::Now().GetSeconds();
    double elapsed=now-g_lastTime;

    double thrFlow1=g_bytesFlow1*8/elapsed/1000000;
    double thrFlow2=g_bytesFlow2*8/elapsed/1000000;
    double thrFlow3=g_bytesFlowBack*8/elapsed/1000000;

    if(thrFlow1>0){
    NS_LOG_UNCOND("1\t"<<Simulator::Now().GetSeconds()
    <<"\t"<<thrFlow1);}
    if(thrFlow2>0){
    NS_LOG_UNCOND("2\t"<<Simulator::Now().GetSeconds()
    <<"\t"<<thrFlow2);}
    if(thrFlow3>0){
    NS_LOG_UNCOND("Back\t"<<Simulator::Now().GetSeconds()
    <<"\t"<<thrFlow3);}

    g_bytesFlow1=0;
    g_bytesFlow2=0;
    g_bytesFlowBack=0;
    g_lastTime=now;

    Simulator::Schedule(Seconds(0.1),&MeasureThroughput);
}

int main(int argc, char* argv[]){

    CommandLine cmd;
    cmd.Parse(argc,argv);

    NS_LOG_INFO("Create nodes.");
    
    NodeContainer csmaNodes1, csmaNodes2, p2pNodes;
    csmaNodes1.Create(2);
    csmaNodes2.Create(2);
    p2pNodes.Create(2);

    NodeContainer link0Nodes;
    link0Nodes.Add(csmaNodes1.Get(0));
    link0Nodes.Add(p2pNodes.Get(0));

    NodeContainer link1Nodes;
    link1Nodes.Add(csmaNodes2.Get(0));
    link1Nodes.Add(p2pNodes.Get(1));

    NodeContainer link2Nodes;
    link2Nodes.Add(csmaNodes1.Get(1));
    link2Nodes.Add(p2pNodes.Get(0));

    NodeContainer link3Nodes;
    link3Nodes.Add(csmaNodes2.Get(1));
    link3Nodes.Add(p2pNodes.Get(1));

    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate",StringValue("100Mbps"));
    csma.SetChannelAttribute("Delay",TimeValue(MicroSeconds(10)));

    NetDeviceContainer link0Devices, link1Devices, link2Devices, link3Devices;
    link0Devices=csma.Install(link0Nodes);
    link1Devices=csma.Install(link1Nodes);
    link2Devices=csma.Install(link2Nodes);
    link3Devices=csma.Install(link3Nodes);

    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate",StringValue("50Mbps"));
    pointToPoint.SetChannelAttribute("Delay",TimeValue(MilliSeconds(10)));
   

    NetDeviceContainer p2pDevices;
    p2pDevices=pointToPoint.Install(p2pNodes);

    InternetStackHelper stack;
    stack.Install(csmaNodes1);
    stack.Install(csmaNodes2);
    stack.Install(p2pNodes);

    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer link0Interfaces;
    link0Interfaces=address.Assign(link0Devices);

    address.SetBase("10.1.2.0","255.255.255.0");
    Ipv4InterfaceContainer link1Interfaces;
    link1Interfaces=address.Assign(link1Devices);

    address.SetBase("10.1.3.0","255.255.255.0");
    Ipv4InterfaceContainer link2Interfaces;
    link2Interfaces=address.Assign(link2Devices);

    address.SetBase("10.1.4.0","255.255.255.0");
    Ipv4InterfaceContainer link3Interfaces;
    link3Interfaces=address.Assign(link3Devices);

    address.SetBase("10.1.5.0","255.255.255.0");
    Ipv4InterfaceContainer p2pInterfaces;
    p2pInterfaces=address.Assign(p2pDevices);

    // Create an OnOff application to send datagrams
    NS_LOG_INFO("Create Application");
    uint16_t port=9;
    OnOffHelper onoff("ns3::UdpSocketFactory",
        Address(InetSocketAddress(Ipv4Address(link1Interfaces.GetAddress(0)),port)));
        onoff.SetAttribute("DataRate",DataRateValue(70000000));
        onoff.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=9999]"));
        onoff.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));
    
    ApplicationContainer app1=onoff.Install(link2Nodes.Get(0));
    app1.Start(Seconds(4.0));
    app1.Stop(Seconds(25.0));
    
    
    onoff.SetAttribute("Remote",
        AddressValue(InetSocketAddress(Ipv4Address(link1Interfaces.GetAddress(0)),port+1)));
        onoff.SetAttribute("DataRate",DataRateValue(90000000));
        onoff.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=9999]"));
        onoff.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));
    
    ApplicationContainer app2=onoff.Install(link0Nodes.Get(0));
    app2.Start(Seconds(10.0));
    app2.Stop(Seconds(20.0));
    

    onoff.SetAttribute("Remote",
        AddressValue(InetSocketAddress(Ipv4Address(link0Interfaces.GetAddress(0)),port+2)));
        onoff.SetAttribute("DataRate",DataRateValue(100000000));
        onoff.SetAttribute("OnTime",StringValue("ns3::ConstantRandomVariable[Constant=9999]"));
        onoff.SetAttribute("OffTime",StringValue("ns3::ConstantRandomVariable[Constant=0.0]"));

    ApplicationContainer app3=onoff.Install(link3Nodes.Get(0));
    app3.Start(Seconds(12.0));
    app3.Stop(Seconds(28.0));

    PacketSinkHelper sinkBack("ns3::UdpSocketFactory",
        Address(InetSocketAddress(Ipv4Address::GetAny(),port)));
    ApplicationContainer sinkApp1=sinkBack.Install(link1Nodes.Get(0));
    sinkApp1.Start(Seconds(4));
    sinkApp1.Get(0)->TraceConnect("Rx","FlowBack",MakeCallback(&Rxtime));

    PacketSinkHelper sink1("ns3::UdpSocketFactory",
        Address(InetSocketAddress(Ipv4Address::GetAny(),port+1)));
    ApplicationContainer sinkApp2=sink1.Install(link1Nodes.Get(0));
    sinkApp2.Start(Seconds(10));
    sinkApp2.Get(0)->TraceConnect("Rx","Flow1",MakeCallback(&Rxtime));

    PacketSinkHelper sink2("ns3::UdpSocketFactory",
        Address(InetSocketAddress(Ipv4Address::GetAny(),port+2)));
     ApplicationContainer sinkApp3=sink2.Install(link0Nodes.Get(0));
    sinkApp3.Start(Seconds(12));
    sinkApp3.Get(0)->TraceConnect("Rx","Flow2",MakeCallback(&Rxtime));
    
    csma.EnablePcapAll("assignment01");

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
 
    Simulator::Schedule(Seconds(0.1),&MeasureThroughput);
  
    NS_LOG_INFO("Run Simulation.");
    Simulator::Stop(Seconds(30));
    Simulator::Run();
    Simulator::Destroy();
    NS_LOG_INFO("Done.");
}