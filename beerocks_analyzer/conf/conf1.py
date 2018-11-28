CMD=[]

#Plot 0
CMD.append("-plot00=ap*_packets_sent")
CMD.append("-plot01=ap*_packets_received")
#Plot 0 - Titles
CMD.append("-fig_title00=AP_Packets_Sent")
CMD.append("-fig_title01=AP_Packets_Received")

#Plot 1
CMD.append("-plot10=ap*_sta*_signal_strength")
CMD.append("-plot11=ap*_sta*_last_data_downlink_rate")
CMD.append("-plot12=ap*_sta*_last_data_uplink_rate")
CMD.append("-plot13=ap*_sta*_bytes_sent")
CMD.append("-plot14=ap*_sta*_bytes_received")
#Plot 1 - Titles
CMD.append("-fig_title10=STA_Signal_Strength")
CMD.append("-fig_title11=STA_Last_Data_Downlink_Rate")
CMD.append("-fig_title12=STA_Last_Data_Uplink_Rate")
CMD.append("-fig_title13=STA_Bytes_Sent")
CMD.append("-fig_title14=STA_Bytes_Received")
