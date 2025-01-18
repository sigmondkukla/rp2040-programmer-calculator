# exec(open("C:\\Users\\Sigmond\\Documents\\KiCad\\Projects\\rp2040-programmer-calculator\\refselect.py").read())

import pcbnew

board = pcbnew.GetBoard()

for footprint in board.GetFootprints():
    ref = footprint.GetReference()
    if ref[0:1] != "SW":
        continue
    ref_num = int(ref[2:])  # Extract the number part of the reference
    if 33 <= ref_num <= 62:
        print("Selected " + str(ref_num))
        footprint.SetSelected()

pcbnew.Refresh() # Refresh the PCB view to show the selection