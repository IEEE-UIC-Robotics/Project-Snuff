package ieee.uic.robotics;

import java.awt.event.ActionListener;

import javax.swing.JFrame;

@SuppressWarnings("serial")
public abstract class SnuffCommandModule extends JFrame implements ActionListener {
	
	private final SnuffCommandCenter SCC;
	
	SnuffCommandModule(SnuffCommandCenter scc) {
		SCC = scc;
	}
	
	public abstract void processResponse(byte[] response);
}
