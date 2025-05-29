#include "ns3/lte-helper.h"
#include "ns3/epc-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/config-store.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("LteMulticell");

	int
main (int argc, char *argv[])
{

	uint16_t numberOfNodes = 2;
	double simTime = 1.1;
	double interPacketInterval = 1;

	// Command line arguments
	CommandLine cmd;
	cmd.AddValue("numberOfNodes", "Number of eNodeBs + UE pairs", numberOfNodes);
	cmd.AddValue("simTime", "Total duration of the simulation [s])", simTime);
	cmd.AddValue("interPacketInterval", "Inter packet interval [ms])", interPacketInterval);
	cmd.Parse(argc, argv);

	Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
	Ptr<PointToPointEpcHelper>  epcHelper = CreateObject<PointToPointEpcHelper> ();
	lteHelper->SetEpcHelper (epcHelper);

	Ptr<Node> pgw = epcHelper->GetPgwNode ();

	// Create a single RemoteHost
	NodeContainer remoteHostContainer;
	remoteHostContainer.Create (1);
	Ptr<Node> remoteHost = remoteHostContainer.Get (0);
	InternetStackHelper internet;
	internet.Install (remoteHostContainer);

	// Create the Internet
	PointToPointHelper p2ph;
	p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
	p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
	p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
	NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
	Ipv4AddressHelper ipv4h;
	ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
	Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);

	Ipv4StaticRoutingHelper ipv4RoutingHelper;
	Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
	remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);

	NodeContainer ueNodes;
	NodeContainer enbNodes;
	enbNodes.Create(numberOfNodes);
	ueNodes.Create(3*numberOfNodes);

	// Install Mobility Model

	// ToDo 1: Make topology
	// ----------------------------------------------- //
	Ptr<ListPositionAllocator> positionAlloc=CreateObject<ListPositionAllocator>();
	positionAlloc->Add(Vector(-100.0,0.0,0.0));
	positionAlloc->Add(Vector(100.0,0.0,0.0));
	positionAlloc->Add(Vector(-100.0,10.0,0.0));
	positionAlloc->Add(Vector(100.0,10.0,0.0));
	positionAlloc->Add(Vector(-100.0,-100.0,0.0));
	positionAlloc->Add(Vector(100.0,-100.0,0.0));
	positionAlloc->Add(Vector(-1.0,0.0,0.0));
	positionAlloc->Add(Vector(1.0,0.0,0.0));

	MobilityHelper mobility;
	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
	mobility.SetPositionAllocator(positionAlloc);
	mobility.Install(enbNodes);
	mobility.Install(ueNodes);

	// ----------------------------------------------- //

	lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");

	// Install LTE Devices to the nodes
	NetDeviceContainer enbLteDevs = lteHelper->InstallEnbDevice (enbNodes);
	NetDeviceContainer ueLteDevs = lteHelper->InstallUeDevice (ueNodes);

	// Install the IP stack on the UEs
	internet.Install (ueNodes);
	Ipv4InterfaceContainer ueIpIface;
	ueIpIface = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueLteDevs));
	// Assign IP address to UEs, and install applications
	for (uint32_t u = 0; u < ueNodes.GetN (); ++u)
	{
		Ptr<Node> ueNode = ueNodes.Get (u);
		// Set the default gateway for the UE
		Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ueNode->GetObject<Ipv4> ());
		ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
	}

	// Attach one UE per eNodeB

	// ToDo 2: Attaching eNB 1 to UE 1,3,5 and eNB 2 to UE 2,4,6
	// ---------------------------------------------------- //
	for(uint16_t i=0;i<numberOfNodes;i++){
		lteHelper->Attach(ueLteDevs.Get(i),enbLteDevs.Get(i));
		lteHelper->Attach(ueLteDevs.Get(i+2),enbLteDevs.Get(i));
		lteHelper->Attach(ueLteDevs.Get(i+4),enbLteDevs.Get(i));
	}



	// ---------------------------------------------------- //

	// Install and start applications on UEs and remote host

	// ToDo 3: Install and start applications on UEs
	// --------------------------------------------------- //
	uint16_t dlPort=1234;
	ApplicationContainer clientApps;
	ApplicationContainer serverApps;
	for(uint32_t u=0;u<ueNodes.GetN();++u){
		PacketSinkHelper dlPacketSinkHelper("ns3::UdpSocketFactory",
									InetSocketAddress(Ipv4Address::GetAny(),dlPort+u));
		serverApps.Add(dlPacketSinkHelper.Install(ueNodes.Get(u)));

		UdpClientHelper dlClientHelper (ueIpIface.GetAddress (u),dlPort+u);
		dlClientHelper.SetAttribute("Interval",TimeValue(MilliSeconds(interPacketInterval)));
		dlClientHelper.SetAttribute("MaxPackets",UintegerValue(1000000));

		clientApps.Add(dlClientHelper.Install(remoteHostContainer.Get(0)));
	}


	// ---------------------------------------------------- //

	serverApps.Start (Seconds (0.01));
	clientApps.Start (Seconds (0.01));
	lteHelper->EnableTraces ();
	// Uncomment to enable PCAP tracing
	//p2ph.EnablePcapAll("lena-epc-first");

	Simulator::Stop(Seconds(simTime));
	Simulator::Run();

	// Ptr<PacketSink> sink = serverApps.Get (0) -> GetObject<PacketSink> ();
	// NS_LOG_UNCOND("UE1 throughput :" << sink->GetTotalRx()*8.0 / 1000000.0 << " Mbps");

	// Ptr<PacketSink> sink1 = serverApps.Get (1) -> GetObject<PacketSink> ();
	// NS_LOG_UNCOND("UE2 throughput :" << sink1->GetTotalRx()*8.0 / 1000000.0 << " Mbps");

	//ToDo 4: Throughput calculation for UE 3 ~ UE 6
	// --------------------------------------------------- //

	Ptr<PacketSink> sink;
	for(int i=0;i<6;i++){
		sink=serverApps.Get(i)->GetObject<PacketSink>();
		NS_LOG_UNCOND("UE"<<i+1<<" throughput :"<<sink->GetTotalRx()*8.0/1000000.0<<"Mbps");
	}




	// ---------------------------------------------------- //

	Simulator::Destroy();
	return 0;

}

