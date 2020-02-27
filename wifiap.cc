/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/ssid.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ThirdScriptExample");

int 
main (int argc, char *argv[])
{
      LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO);
      LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);
  //création
  NodeContainer p2pNodes1,p2pNodes2;

  NodeContainer csmaNodes1,csmaNodes2,csmaNodes3;

  csmaNodes1.Create (3);
  csmaNodes2.Create (3);
  csmaNodes3.Create (3);

  p2pNodes1.Add(csmaNodes1.Get(2));
  p2pNodes1.Add(csmaNodes2.Get(0));

  p2pNodes2.Add(csmaNodes2.Get(2));
  p2pNodes2.Add(csmaNodes3.Get(2));

  NodeContainer wifiStaNodes1,wifiStaNodes2;
  wifiStaNodes1.Create (4);
  wifiStaNodes2.Create (2);
  NodeContainer wifiAp1 = csmaNodes1.Get(0);
  NodeContainer wifiAp2 = csmaNodes2.Get(0);

  //////////////////////////////////////////////////////////////////
  //Configuration de partie de wifi
  YansWifiChannelHelper channel1 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy1 = YansWifiPhyHelper::Default ();
  phy1.SetChannel (channel1.Create ());
  ///
  YansWifiChannelHelper channel2 = YansWifiChannelHelper::Default ();
  YansWifiPhyHelper phy2 = YansWifiPhyHelper::Default ();
  phy2.SetChannel (channel2.Create ());
//////
  WifiHelper wifi;
  wifi.SetRemoteStationManager ("ns3::AarfWifiManager");
 WifiMacHelper mac1;
  Ssid ssid1 = Ssid ("ns-3-ssid");
  mac1.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid1));
 WifiMacHelper mac2; 
  Ssid ssid2 = Ssid ("ns-3-ssid2");
  mac2.SetType ("ns3::StaWifiMac", "Ssid", SsidValue (ssid2));
//////
NetDeviceContainer staDevices1,staDevices2,apDevices1,apDevices2;
  staDevices1 = wifi.Install  (phy1, mac1, wifiStaNodes1);
  mac1.SetType ("ns3::ApWifiMac",
              "Ssid", SsidValue (ssid1));
  apDevices1 = wifi.Install (phy1, mac1, wifiAp1);  
  /////
  staDevices2 = wifi.Install  (phy2, mac2, wifiStaNodes2);
  mac2.SetType ("ns3::ApWifiMac",
              "Ssid", SsidValue (ssid2));
  apDevices2 = wifi.Install (phy2, mac2, wifiAp2);
 /////
  MobilityHelper mobility,mobility2;

  mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility.Install (wifiStaNodes1);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (wifiAp1);
  ///
   mobility2.SetPositionAllocator ("ns3::GridPositionAllocator",
                                 "MinX", DoubleValue (0.0),
                                 "MinY", DoubleValue (0.0),
                                 "DeltaX", DoubleValue (5.0),
                                 "DeltaY", DoubleValue (10.0),
                                 "GridWidth", UintegerValue (3),
                                 "LayoutType", StringValue ("RowFirst"));

  mobility2.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
                             "Bounds", RectangleValue (Rectangle (-50, 50, -50, 50)));
  mobility2.Install (wifiStaNodes2);
  mobility2.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility2.Install (wifiAp2);
  ///////////////////////////////////////////////////////////////////////////////////////
  ///Configuration de partie pointtopoint et csma
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer p2pDevices1,p2pDevices2;
  p2pDevices1 = pointToPoint.Install (p2pNodes1);
  p2pDevices2 = pointToPoint.Install (p2pNodes2);

  CsmaHelper csma;
  csma.SetChannelAttribute ("DataRate", StringValue ("100Mbps"));
  csma.SetChannelAttribute ("Delay", TimeValue (NanoSeconds (6560)));

  NetDeviceContainer csmaDevices1,csmaDevices2,csmaDevices3;
  csmaDevices1 = csma.Install(csmaNodes1);
  csmaDevices2 = csma.Install(csmaNodes2);
  csmaDevices3 = csma.Install(csmaNodes3);
  /////////////////////////////////////////////////////////////////////////////////////
  //La pile
  InternetStackHelper stack;
  stack.Install (csmaNodes1);
  stack.Install (csmaNodes2);
  stack.Install (csmaNodes3);
  stack.Install (wifiStaNodes1);
  stack.Install (wifiStaNodes2);
  //////////////////////////////////////////////////////////////////////////////////////
  //Les adresses
  Ipv4AddressHelper address;

  address.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces1;
  p2pInterfaces1 = address.Assign (p2pDevices1);

  address.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer p2pInterfaces2;
  p2pInterfaces2 = address.Assign (p2pDevices2);

  address.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces1;
  csmaInterfaces1 = address.Assign (csmaDevices1);

  address.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces2;
  csmaInterfaces2 = address.Assign (csmaDevices2);

  address.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer csmaInterfaces3;
  csmaInterfaces3 = address.Assign (csmaDevices3);

  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer wifistaInterfaces1 = address.Assign (staDevices1);
  Ipv4InterfaceContainer wifiapInterfaces1 = address.Assign (apDevices1);

  address.SetBase ("10.1.7.0", "255.255.255.0");
  Ipv4InterfaceContainer wifistaInterfaces2 = address.Assign (staDevices2);
  Ipv4InterfaceContainer wifiapInterfaces2 = address.Assign (apDevices2);
  
  /////////////////////////////////////////////////////////////////////////////////
  //Les applications
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install (wifiStaNodes1.Get (0));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (10.0));

  UdpEchoClientHelper echoClient (wifistaInterfaces1.GetAddress (0), 9);
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
  ////////////////////////////////////////////////////////////////
  ApplicationContainer clientApps = echoClient.Install (wifiStaNodes2.Get(1));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (10.0));

  ApplicationContainer clientApps2 = echoClient.Install (csmaNodes2.Get(2));
  clientApps2.Start (Seconds (2.0));
  clientApps2.Stop (Seconds (10.0));

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Stop (Seconds (10.0));
  
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
