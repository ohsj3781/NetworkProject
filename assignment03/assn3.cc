/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Assignment 3 - Selective Reliable UDP Echo Application
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/udp-reliable-echo-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("Assn3");

int main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  

  LogComponentEnable ("UdpReliableEchoClientApplication", LOG_LEVEL_INFO);
  
  // Create nodes
  NodeContainer nodes;
  nodes.Create (4);

  InternetStackHelper stack;
  stack.Install (nodes);
  
  
  // Create point-to-point links
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("5ms"));
  pointToPoint.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("1500B"));
  
  // Create network devices and install on nodes
  NetDeviceContainer devices01 = pointToPoint.Install (nodes.Get(0), nodes.Get(1));
  NetDeviceContainer devices12 = pointToPoint.Install (nodes.Get(1), nodes.Get(2));
  NetDeviceContainer devices13 = pointToPoint.Install (nodes.Get(1), nodes.Get(3));
 
  // Assign IP addresses
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces01 = address.Assign (devices01);
  
  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces12 = address.Assign (devices12);
  
  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces13 = address.Assign (devices13);
  
  // Enable routing
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
  
  // Set error rate
  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.001));
  devices01.Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
  
  // Create and configure UdpReliableEchoServer on node 2
  uint16_t echoPort = 9;
  Address reliableAddress(InetSocketAddress(interfaces12.GetAddress(1),echoPort));
  UdpReliableEchoServerHelper echoServer (echoPort);
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (2));
  serverApps.Start (Seconds (0.0));
  serverApps.Stop (Seconds (61.0));
  
  // Create and configure UdpReliableEchoClient on node 0
  UdpReliableEchoClientHelper echoClient (reliableAddress, echoPort);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.0001)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  
  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (1.0));
  clientApps.Stop (Seconds (60.0));
  
  // Create and configure OnOffApplication on node 0
  uint16_t onoffPort = 10;
  Address sinkAddress (InetSocketAddress(interfaces13.GetAddress(1),onoffPort));
  OnOffHelper onOffHelper ("ns3::UdpSocketFactory",sinkAddress);
  onOffHelper.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.3]"));
  onOffHelper.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=3]"));
  onOffHelper.SetAttribute ("DataRate", DataRateValue (DataRate ("10Mbps")));
  onOffHelper.SetAttribute ("PacketSize", UintegerValue (512));
  
  ApplicationContainer onOffApps = onOffHelper.Install (nodes.Get (0));
  onOffApps.Start (Seconds (1.0));
  onOffApps.Stop (Seconds (60.0));
  
  // Create PacketSink on node 3
  PacketSinkHelper packetSinkHelper ("ns3::UdpSocketFactory",sinkAddress);
  ApplicationContainer sinkApps = packetSinkHelper.Install (nodes.Get (3));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (61.0));
  
  // Run simulation
  Simulator::Stop (Seconds (63.0));
  Simulator::Run ();
  Simulator::Destroy ();
  
  return 0;
}