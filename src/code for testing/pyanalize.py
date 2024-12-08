import serial
import csv
import time
import datetime
import matplotlib.pyplot as plt
import numpy as np
import scipy.interpolate
import argparse


def parse_and_save_data(arduino, file_name="arduino_data.csv", duration=30):
    """
    Read, parse, save data from the Arduino, and display it on the terminal.
    """
    start_time = time.time()
    data_entries = []

    print(f"Reading data and saving to {file_name}...")

    with open(file_name, "w", newline='') as file:
        headers_written = False

        while time.time() - start_time < duration:
            try:
                # Read a line of data from the Arduino
                line = arduino.readline().decode('utf-8').strip()
                if line.startswith("[") and line.endswith("]"):
                    # Strip brackets and split by commas
                    line = line[1:-1]
                    values = line.split(",")
                    # Ensure all 8 values are present
                    if len(values) == 8:
                        # Prepare a dictionary with parsed data
                        data_dict = {
                            "Timestamp": datetime.datetime.now().strftime('%H:%M:%S.%f')[:-3],
                            "State": int(values[0]),
                            "Yaw": float(values[1]),
                            "Way_Dist": float(values[2]),
                            "Way_Angle": float(values[3]),
                            "MotorA": int(values[4]),
                            "MotorB": int(values[5]),
                            "YawPIDOutput": float(values[6]),
                            "RotationPIDOutput": float(values[7])
                        }

                        if not headers_written:
                            # Write headers
                            writer = csv.DictWriter(file, fieldnames=data_dict.keys())
                            writer.writeheader()
                            headers_written = True

                        # Write row to CSV file
                        writer.writerow(data_dict)

                        # Print to terminal
                        print(" | ".join(f"{key}: {value}" for key, value in data_dict.items()))
                        data_entries.append(data_dict)
                    else:
                        print(f"Malformed data: {line}")
                else:
                    print(f"Ignoring invalid line: {line}")
            except ValueError:
                print("Received non-numeric or malformed data; skipping.")
            except serial.SerialException as e:
                print(f"Serial error: {e}")
                break

    print(f"Data saved to {file_name}. Total entries: {len(data_entries)}.")
    return file_name

def plot_data(file_name):
    """
    Plot data from a CSV file using timestamps as the x-axis.
    Use interpolation to plot data as continuous functions.
    """
    print(f"Plotting data from {file_name}...")
    
    with open(file_name, "r") as file:
        reader = csv.DictReader(file)
        data = {header: [] for header in reader.fieldnames}

        # Read the data
        for row in reader:
            for key in row:
                data[key].append(row[key])

    # Convert timestamps to matplotlib datetime objects
    if "Timestamp" in data:
        time_x = [datetime.datetime.strptime(ts, '%H:%M:%S.%f') for ts in data["Timestamp"]]
        # Convert to seconds since start time for easier interpolation
        time_seconds = [(t - time_x[0]).total_seconds() for t in time_x]
    else:
        print("Timestamp data missing. Cannot plot against time.")
        return

    # Prepare a fine time range for smooth plotting
    time_fine = np.linspace(time_seconds[0], time_seconds[-1], 1000)

    # Plot figure
    plt.figure(figsize=(15, 12))

    # Plot State as a function of time
    if "State" in data:
        state_interp = scipy.interpolate.interp1d(time_seconds, data["State"], kind="cubic", fill_value="extrapolate")
        state_smooth = state_interp(time_fine)
        plt.subplot(4, 2, 1)
        plt.plot(time_fine, state_smooth, label="State", color="blue")
        plt.xlabel("Time")
        plt.ylabel("State")
        plt.title("State Over Time")
        plt.grid()
        plt.legend()

    # Plot Yaw as a function of time
    if "Yaw" in data:
        yaw_interp = scipy.interpolate.interp1d(time_seconds, data["Yaw"], kind="cubic", fill_value="extrapolate")
        yaw_smooth = yaw_interp(time_fine)
        plt.subplot(4, 2, 2)
        plt.plot(time_fine, yaw_smooth, label="Yaw", color="cyan")
        plt.xlabel("Time")
        plt.ylabel("Yaw")
        plt.title("Yaw Over Time")
        plt.grid()
        plt.legend()

    # Plot Way_Dist as a function of time
    if "Way_Dist" in data:
        way_dist_interp = scipy.interpolate.interp1d(time_seconds, data["Way_Dist"], kind="cubic", fill_value="extrapolate")
        way_dist_smooth = way_dist_interp(time_fine)
        plt.subplot(4, 2, 3)
        plt.plot(time_fine, way_dist_smooth, label="Way Distance", color="orange")
        plt.xlabel("Time")
        plt.ylabel("Way Distance")
        plt.title("Way Distance Over Time")
        plt.grid()
        plt.legend()

    # Plot MotorA and MotorB as a function of time
    if "MotorA" in data and "MotorB" in data:
        motorA_interp = scipy.interpolate.interp1d(time_seconds, data["MotorA"], kind="cubic", fill_value="extrapolate")
        motorA_smooth = motorA_interp(time_fine)
        motorB_interp = scipy.interpolate.interp1d(time_seconds, data["MotorB"], kind="cubic", fill_value="extrapolate")
        motorB_smooth = motorB_interp(time_fine)
        plt.subplot(4, 2, 4)
        plt.plot(time_fine, motorA_smooth, label="MotorA", color="green")
        plt.plot(time_fine, motorB_smooth, label="MotorB", color="red")
        plt.xlabel("Time")
        plt.ylabel("Motor Speeds")
        plt.title("Motor Speeds Over Time")
        plt.grid()
        plt.legend()

    # Plot YawPIDOutput as a function of time
    if "YawPIDOutput" in data:
        yawPID_interp = scipy.interpolate.interp1d(time_seconds, data["YawPIDOutput"], kind="cubic", fill_value="extrapolate")
        yawPID_smooth = yawPID_interp(time_fine)
        plt.subplot(4, 2, 5)
        plt.plot(time_fine, yawPID_smooth, label="Yaw PID Output", color="purple")
        plt.xlabel("Time")
        plt.ylabel("PID Output")
        plt.title("Yaw PID Output Over Time")
        plt.grid()
        plt.legend()

    # Plot RotationPIDOutput as a function of time
    if "RotationPIDOutput" in data:
        rotationPID_interp = scipy.interpolate.interp1d(time_seconds, data["RotationPIDOutput"], kind="cubic", fill_value="extrapolate")
        rotationPID_smooth = rotationPID_interp(time_fine)
        plt.subplot(4, 2, 6)
        plt.plot(time_fine, rotationPID_smooth, label="Rotation PID Output", color="brown")
        plt.xlabel("Time")
        plt.ylabel("PID Output")
        plt.title("Rotation PID Output Over Time")
        plt.grid()
        plt.legend()

    plt.tight_layout()
    plt.show()


def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(description="Arduino Data Collection and Plotting")
    parser.add_argument("file_name", nargs="?", help="CSV file to plot (optional)")

    args = parser.parse_args()

    # If a file name is provided, plot the data from that file
    if args.file_name:
        plot_data(file_name=args.file_name)
    else:
        # Otherwise, connect to Arduino and collect data
        arduino = serial.Serial(port='COM6', baudrate=115200, timeout=1)
        file_name = parse_and_save_data(arduino, duration=40)
        plot_data(file_name)


if __name__ == "__main__":
    main()
