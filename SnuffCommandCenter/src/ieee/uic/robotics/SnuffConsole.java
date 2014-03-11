package ieee.uic.robotics;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextPane;
import javax.swing.border.BevelBorder;
import javax.swing.text.BadLocationException;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyledDocument;

public class SnuffConsole extends JPanel {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private JTextPane console;
	private JScrollPane scrollPane;
	private StyledDocument doc;
	private Style style;

	public SnuffConsole() {
		super();
		setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
		console = new JTextPane();
		scrollPane = new JScrollPane();
		doc = console.getStyledDocument();
		style = console.addStyle("STYLE", null);
		scrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
		scrollPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
		setUpConsole();
		setUpClearButton();
	}

	private void setUpClearButton() {

        JPanel statusPanel = new JPanel();
        statusPanel.setBorder(new BevelBorder(BevelBorder.LOWERED));
        this.add(statusPanel, BorderLayout.SOUTH);
        statusPanel.setPreferredSize(new Dimension(this.getWidth(), 20));
        statusPanel.setLayout(new BoxLayout(statusPanel, BoxLayout.X_AXIS));
        JButton button = new JButton("Clear");
        button.addMouseListener(new MouseListener() {
			@Override
			public void mouseReleased(MouseEvent e) {}
			
			@Override
			public void mousePressed(MouseEvent e) {}
			
			@Override
			public void mouseExited(MouseEvent e) {}
			
			@Override
			public void mouseEntered(MouseEvent e) {}
			
			@Override
			public void mouseClicked(MouseEvent e) {
				console.setText("");
			}
		});
        statusPanel.add(button);
	}

	private void setUpConsole() {
		console.setEditable(false);
		console.setFont(new Font("Courier New", Font.PLAIN, 12));
		console.setBackground(Color.BLACK);
		console.setForeground(Color.GREEN);
		console.setMargin(new Insets(5, 5, 5, 5));
		scrollPane.setViewportView(console);
		add(scrollPane);
	}

	public void pl(Color color,String line) {
		StyleConstants.setForeground(style, color);
		print(line+"\n");
	}

	public void p(Color color,String line) {
		StyleConstants.setForeground(style, color);
		print(line);
	}
	
	private void print(String line){
		try {
			doc.insertString(doc.getLength(), line, style);
		} catch (BadLocationException e) {
		}
	}
}
