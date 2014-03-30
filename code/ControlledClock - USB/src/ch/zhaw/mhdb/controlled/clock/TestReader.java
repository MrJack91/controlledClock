/**
 * 
 */
package ch.zhaw.mhdb.controlled.clock;

import java.io.IOException;
import java.util.Arrays;
import java.util.Date;
import java.util.TooManyListenersException;

import jd2xx.JD2XX;
import jd2xx.JD2XX.ProgramData;
import jd2xx.JD2XXEvent;
import jd2xx.JD2XXEventListener;
import jd2xx.JD2XXInputStream;
import jd2xx.JD2XXOutputStream;

/**
 * @author Daniel Brun
 * 
 */
public class TestReader implements JD2XXEventListener {

	public TestReader() {
		JD2XX jd = new JD2XX();
		
		jd.reload(0x0403, 0xe88a);

		Object[] devs;
		try {
			devs = jd.listDevicesBySerialNumber();

			for (int i = 0; i < devs.length; ++i) {
				System.out.println(devs[i]);
			}

			devs = jd.listDevicesByDescription();
			for (int i = 0; i < devs.length; ++i) {
				System.out.println(devs[i]);
			}
			
			devs = jd.listDevicesByLocation();
			for (int i=0; i<devs.length; ++i)
			  System.out.println(
			    Integer.toHexString((Integer)devs[i])
			  );
			
			jd.addEventListener(this);
			jd.open(0);
			jd.setBaudRate(6000);
			//jd.setLatencyTimer(465);
			jd.setDataCharacteristics(
			  JD2XX.BITS_8, JD2XX.STOP_BITS_1, JD2XX.PARITY_NONE
			);
//			jd.setFlowControl(
//			  JD2XX.FLOW_NONE, 0, 0
//			);
			//jd.setTimeouts(500, 500);
			
			byte[] rd = jd.read(10);
			System.out.println(new String(rd));
			
			ProgramData pd = jd.eeRead();
			System.out.println(pd.toString());
			
			JD2XXInputStream ins = new JD2XXInputStream(jd);
//			JD2XXOutputStream outs = new JD2XXOutputStream(jd);
//			
			byte[] data = new byte[1];
			int ret = -1;
			while((ret = ins.read(data)) != -1){
				if(ret > 0){
					
					switch (data[0]){
					
					case -128:
					case 0:
						System.out.println("Bit-Code: 1 (" + data[0] + ", " + new Date() + ")");
						break;
						
					case -8:
					case -16:
					case -32:
						System.out.println("Bit-Code: 0 (" + data[0] + ", " + new Date() + ")");
						break;
					default: {
						System.out.println("Undefinied: " + data[0] + " (" + new Date() + ")");
					}
					}
				}
				
			}
//			outs.write(data);
			
			//224 / -96	/
			//240 / -112	/
			//248 / -120	/ -7
			//0				
			//128 / ...		/ -128
			
			ins.close();
//			outs.jd2xx.close();
//			outs.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (TooManyListenersException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}finally{
			
		}

	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		new TestReader();

	}

	@Override
	public void jd2xxEvent(JD2XXEvent event) {
		System.out.println("Got Event " + event.getEventType());
	}

}
