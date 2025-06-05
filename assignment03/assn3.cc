#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "udp-reliable-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ReliableUdpExample");

int
main (int argc, char *argv[])
{
  // Enable logging
  LogComponentEnable ("UdpReliableEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpReliableEchoServerApplication", LOG_LEVEL_INFO);

  // Create nodes
  NodeContainer nodes;
  nodes.Create (4);

  NodeContainer n0n1 = NodeContainer (nodes.Get (0), nodes.Get (1));
  NodeContainer n1n2 = NodeContainer (nodes.Get (1), nodes.Get (2));
  NodeContainer n1n3 = NodeContainer (nodes.Get (1), nodes.Get (3));

  // Create the point-to-point links
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("5ms"));
  pointToPoint.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue ("1500B"));

  NetDeviceContainer d0d1 = pointToPoint.Install (n0n1);
  NetDeviceContainer d1d2 = pointToPoint.Install (n1n2);
  NetDeviceContainer d1d3 = pointToPoint.Install (n1n3);

  // Install the internet stack on the nodes
  InternetStackHelper internet;
  internet.Install (nodes);

  // Assign IP addresses to the device containers
  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i1 = address.Assign (d0d1);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i2 = address.Assign (d1d2);

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i3 = address.Assign (d1d3);

  // Enable global routing
  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  // Create the UdpReliableEchoServer application on node 2
  UdpReliableEchoServerHelper echoServer (9);
  ApplicationContainer serverApps = echoServer.Install (nodes.Get (2));
  serverApps.Start (Seconds (0.0));
  serverApps.Stop (Seconds (61.0));

  // Create the UdpReliableEchoClient application on node 0
  UdpReliableEchoClientHelper echoClient (i1i2.GetAddress (1), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1000000));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (0.0001)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));
  clientApps.Start (Seconds (1.0));
  clientApps.Stop (Seconds (60.0));

  // Create the OnOffApplication (traffic generator) on node 0
  OnOffHelper onoff ("ns3::UdpSocketFactory", 
                     Address (InetSocketAddress (i1i3.GetAddress (1), 9)));
  onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.3]"));
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=3]"));
  onoff.SetAttribute ("DataRate", DataRateValue (DataRate ("10Mbps")));
  onoff.SetAttribute ("PacketSize", UintegerValue (512));

  ApplicationContainer onOffApps = onoff.Install (nodes.Get (0));
  onOffApps.Start (Seconds (1.0));
  onOffApps.Stop (Seconds (60.0));

  // Create the PacketSink application on node 3
  PacketSinkHelper sink ("ns3::UdpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), 9)));
  ApplicationContainer sinkApps = sink.Install (nodes.Get (3));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (61.0));

  // Enable error model on the links to simulate packet loss
  Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
  em->SetAttribute ("ErrorRate", DoubleValue (0.001));
  d0d1.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
  d1d2.Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (em));
  d1d3.Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

  // Run the simulation
  Simulator::Stop (Seconds (63.0));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}