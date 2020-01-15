from . import BaseTest, LogType, DeviceType

class test_initial_ap_config(BaseTest):
    def test_initial_ap_config(self):
        self.status("test initial autoconfig")
        self.assertTrue(self.find_in_logs(DeviceType.repeater1, LogType.beerocks_agent_wlan0,
                                      "WSC Global authentication success"))
        self.assertTrue(self.find_in_logs(DeviceType.repeater1, LogType.beerocks_agent_wlan2,
                                      "WSC Global authentication success"))
        self.assertTrue(self.find_in_logs(DeviceType.repeater1, LogType.beerocks_agent_wlan0,
                                       "KWA (Key Wrap Auth) success"))
        self.assertTrue(self.find_in_logs(DeviceType.repeater1, LogType.beerocks_agent_wlan2,
                                      "KWA (Key Wrap Auth) success"))
        self.assertTrue(self.find_in_logs(DeviceType.repeater1, LogType.beerocks_agent_wlan0,
                                      "Controller configuration (WSC M2 Encrypted Settings)"))
        self.assertTrue(self.find_in_logs(DeviceType.repeater1, LogType.beerocks_agent_wlan2,
                                      "Controller configuration (WSC M2 Encrypted Settings)"))
        self.assertTrue(False)
