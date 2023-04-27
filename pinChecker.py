import serial
import tkinter as tk
from tkinter import ttk
from serial.tools import list_ports

ser = None

def set_port(port):
    global ser
    # Update the serial object with the specific port
    ser = serial.Serial(port=port, baudrate=9600, timeout=1)

def toggle_pin(pin_number, value):
    global ser
    if ser is not None and ser.is_open:
        # Send the command to the Arduino
        ser.write(f"{str(pin_number)}:{value}".encode())
        print(f"{pin_number}:{value}".encode())
        #print(f"Pin {pin_number}:{value}")

def setup_gui():
    # Create the GUI window
    window = tk.Tk()
    window.title("Arduino Pin-checker")
    window.geometry("400x700")

    # Create a drop-down menu to select the serial port
    port_label = ttk.Label(window, text="Select Serial Port", font=("Arial Bold", 20))
    port_label.pack()
    port_var = tk.StringVar()
    ports = [port.device for port in list_ports.comports()]
    port_menu = ttk.Combobox(window, width=20, textvariable=port_var, values=ports)
    port_menu.pack()

    # Create two frames for the digital and analog pins
    digital_pins_frame = tk.Frame(window)
    digital_pins_frame.pack(side=tk.LEFT, padx=20, pady=20)
    analog_pins_frame = tk.Frame(window)
    analog_pins_frame.pack(side=tk.LEFT, padx=20, pady=20)

    # Create a switch for each digital pin on the Arduino Uno
    for pin_number in range(2, 14):
        pin_switch_var = tk.IntVar()
        pin_switch = tk.Checkbutton(digital_pins_frame, text=f"Pin {pin_number}", font=("Arial Bold", 20), variable=pin_switch_var, onvalue=1, offvalue=0, command=lambda pin_number=pin_number, pin_switch_var=pin_switch_var: toggle_pin(pin_number, pin_switch_var.get()), padx=30, pady=5)
        pin_switch.pack()

    # Create a switch for each analog pin on the Arduino Uno
    for pin_number in range(6):
        pin_switch_var = tk.IntVar()
        pin_switch = tk.Checkbutton(analog_pins_frame, text=f"A{pin_number}", font=("Arial Bold", 20), variable=pin_switch_var, onvalue=1, offvalue=0, command=lambda pin_number=pin_number, pin_switch_var=pin_switch_var: toggle_pin(f"A{pin_number}", pin_switch_var.get()), padx=30, pady=5)
        pin_switch.pack()

    # Configure the serial port
    port_menu.bind("<<ComboboxSelected>>", lambda event: set_port(port_var.get()))

    # Start the GUI event loop
    window.mainloop()


if __name__ == "__main__":
    setup_gui()
