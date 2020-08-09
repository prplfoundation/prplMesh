import time
from collections import Counter
from boardfarm.exceptions import SkipTest
from .prplmesh_base_test import PrplMeshBaseTest
from capi import tlv
from opts import debug
from sniffer import Sniffer

class LinkMetricQuery(PrplMeshBaseTest):
    """Check if an agent can report the links it formed with other devices properly

    Devices used in test setup:
    AP1 - Agent1 [DUT]
    AP2 - Agent2
    GW - Controller

    Given:
    Bi-directional link is formed between AP1 and GW
    Bi-directional link is formed between AP2 and AP1
    When:
    GW controller is instructed; "Link metric query" (0x0005) is sent for "All neighbors" (0) and
    for "Both Tx and Rx link metrics" (2) from GW controller to AP1 agent
    Then:
    Agent must return a "Link metric response" (0x0006) which contains:
    - A "1905 transmitter link metric" TLV with AP1's local interface MAC address
    and GW's interface MAC address
    - A "1905 receiver link metric" TLV with AP1's local interface MAC address
    and GW's interface MAC address
    - A "1905 transmitter link metric" TLV with AP1's local interface MAC address
    and AP2's interface MAC address
    - A "1905 receiver link metric" TLV with AP1's local interface MAC address
    and AP2's interface MAC address

    Link's media type (ethernet, wifi), packet stats, RSSI (for wireless rx links) and phy rate
    (for wireless tx links) are not checked in this test.
    """
    def runTest(self):
        # skip this test if one of the components does not exist in setup
        try:
            agent1 = self.dev.DUT.agent_entity
            agent2 = self.dev.lan2.agent_entity
            controller = self.dev.lan.controller_entity
        except AttributeError as ae:
            raise SkipTest(ae)

        tcpdump_log_dir = self.dev.DUT.wired_sniffer.tcpdump_log_dir
        sniffer = Sniffer("", tcpdump_log_dir, agent1.name)
        self.dev.DUT.wired_sniffer = sniffer
        sniffer.start(self.__class__.__name__ + "-" + self.dev.DUT.name)

        mid = controller.ucc_socket.dev_send_1905(agent1.mac, 0x0005,
                                                  tlv(0x08, 0x0002, "0x00 0x02"))
        time.sleep(1)

        query = self.check_cmdu_type_single("link metric query", 0x0005,
                                            controller.mac, agent1.mac,
                                            mid)
        query_tlv = self.check_cmdu_has_tlv_single(query, 0x08)

        debug("Checking query type and queried metrics are correct")
        assert int(query_tlv.link_metric_query_type) == 0x00, "Query type is not 'All neighbors'"
        assert int(query_tlv.link_metrics_requested) == 0x02, \
            "Metrics for both Tx and Rx is not requested"

        resp = self.check_cmdu_type_single("link metric response", 0x0006,
                                           agent1.mac, controller.mac,
                                           mid)

        debug("Checking response contains expected links to/from agent and nothing else")
        # neighbor macs are based on the setup in "launch_environment_docker" method
        expected_tx_link_neighbors = [controller.mac, agent2.mac]
        expected_rx_link_neighbors = [controller.mac, agent2.mac]
        actual_tx_links = self.check_cmdu_has_tlvs(resp, 0x09)
        actual_rx_links = self.check_cmdu_has_tlvs(resp, 0x0a)

        def verify_links(links, expected_neighbors, link_type):
            verified_neighbors = []
            unexpected_neighbors = []
            for link in links:
                assert link.responder_mac_addr == agent1.mac, "Responder MAC address is wrong"
                # a tshark (v3.2.4) bug causes "neighbor_mac_addr" field to
                # show up as "responder_mac_addr_2"
                if link.responder_mac_addr_2 in expected_neighbors:
                    verified_neighbors += [link.responder_mac_addr_2]
                else:
                    unexpected_neighbors += [link.responder_mac_addr_2]

            # we expect each neighbor to appear only once
            dup_links = []
            for neighbor, count in Counter(verified_neighbors).items():
                if count != 1:
                    dup_links.append((neighbor, count))
            assert not dup_links, \
                "Following {} links were expected to appear only once:\n".format(link_type)\
                + "\n".join(dup_links)

            # report any extra neighbors that show up
            assert not unexpected_neighbors, \
                "{} links to following neighbors were not expected:\n".format(link_type)\
                + "\n".join(unexpected_neighbors)

        # check responder mac is our own mac, neighbor is one of the expected macs for each link
        verify_links(actual_tx_links, expected_tx_link_neighbors, "tx")
        verify_links(actual_rx_links, expected_rx_link_neighbors, "rx")
