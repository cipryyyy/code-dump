#!/usr/bin/env python3
import sys
import subprocess
import importlib

#libs check, just ignore it
REQUIRED_LIBS = [
    "PIL",
    "tqdm"
]

def ensure_dependencies():
    for lib in REQUIRED_LIBS:
        try:
            importlib.import_module(lib if lib != "PIL" else "PIL.Image")
        except ImportError:
            print(f"Library '{lib}' not found. Installing...")
            install_name = "pillow" if lib == "PIL" else lib.lower()
            subprocess.check_call([sys.executable, "-m", "pip", "install", install_name])

    for lib in REQUIRED_LIBS:
        importlib.import_module(lib if lib != "PIL" else "PIL.Image")
ensure_dependencies()

import os
import platform
from PIL import Image
import re
from tqdm import tqdm

#Main
def main(source, store, removeBG, tolerance):
    print(source, "\n", store)
    dir = os.listdir(source)
    if len(dir) == 0:  # Is it empty?
        print(f"{source} does not contain any files")
        return

    dir = selectAseprite(dir)
    for file in tqdm(dir, desc="Converting", unit="file"):
        #print(f"{source + file}\nvvvvvvvvvvvvvvv\n{store + file.split(".")[0] + '.png'}\n") #DEBUG
        convert(source + file, store + file.split(".")[0] + ".png")
        if removeBG:
            if os.path.exists(store + file.split(".")[0] + ".png"):
                eraseBG(store + file.split(".")[0] + ".png", tolerance)
            else:
                i = 1
                while os.path.exists(store + file.split(".")[0] + str(i) +".png"):
                    eraseBG(store + file.split(".")[0] + str(i) +".png", tolerance)
                    i += 1

#Select only .aseprite
def selectAseprite(source):
    tmp = []
    for file in source:
        if file[0] == ".":              #Ignore hidden files
            continue
        if file[-8:] == "aseprite":     #Select only asprite files
            tmp.append(file)
    return tmp

#Cross-platform (maybe) process to convert the files
def convert(source, store):
    os_name = platform.system()
    
    if os_name == "Darwin":
        aseprite_path = "/Applications/Aseprite.app/Contents/MacOS/aseprite"
    elif os_name == "Linux":
        aseprite_path = "aseprite"
    elif os_name == "Windows":                                #BLEAH >:( 
        aseprite_path = r"C:\Program Files\Aseprite\Aseprite.exe"
    else:
        print(f"What the heck OS are you using???")
        return
    
    while True:
        try:
            subprocess.run([aseprite_path, "-b", source, "--save-as", store], check=True)
            break
        except FileNotFoundError:
            print(f"Aseprite executable not found: {aseprite_path}")
            user_input = input("Enter the full path to the Aseprite executable or 'Q' to quit: ")
            if user_input.lower() == "q":
                return
            aseprite_path = user_input
        except subprocess.CalledProcessError as e:
            print(f"Error during conversion: {e}")
            break

#Remove the background
def eraseBG(file, tolerance):
    input_image = Image.open(file).convert("RGBA")
    pixels = input_image.load()
    width, height = input_image.size

    for i in range(width):
        for j in range(height):
            r, g, b, a = pixels[i, j]
            if r <= tolerance and g <= tolerance and b <= tolerance:
                pixels[i, j] = (0, 0, 0, 0)

    input_image.save(file)

# example:
# ./aseprite-converter.py /Users/RobertoBenigni/fotoPiedi/ /Users/GianniMorandi/piattiDiMerda/ -t=100

if __name__ == "__main__":
    removeBG = True
    tolerance = 12
    debug = False

    if "-k" in sys.argv:                            #Keep the background?
        sys.argv.remove("-k")
        removeBG = False

    if "-d" in sys.argv:                            #Don't mind me :3
        sys.argv.remove("-d")
        debug = True    

    token_pattern = re.compile(r"-t=(\d+)")         #Change the tolerance?
    new_argv = []
    for arg in sys.argv:
        match = token_pattern.fullmatch(arg)
        if match:
            value = int(match.group(1))
            if 0 <= value <= 255:
                tolerance = value
            else:
                print("Error: the value of -t must be between 0 and 255.")
                sys.exit(1)
        else:
            new_argv.append(arg)
    sys.argv = new_argv

    #I/O check
    if len(sys.argv) < 2:
        source = input("Specify the directory containing .aseprite files: ").replace("'", "").replace(" ", "")
    else:
        source = sys.argv[1].replace("'", "").replace(" ", "")
    source = source + "/" if source[-1] != "/" else source

    if (not os.path.exists(source)):
        print(f"Directory {source} does not exist")
        exit()

    if len(sys.argv) < 3:
        store = input("Specify the directory to save .png files: ").replace("'", "").replace(" ", "")
    else:
        store = sys.argv[2].replace("'", "").replace(" ", "")
    store = store + "/" if store[-1] != "/" else store

    if (not os.path.exists(store)):
        choice = input(f"Directory {store} does not exist, create it? (Y/n): ").lower()
        if choice == "n":
            exit()
        os.makedirs(store)

    main(source, store, removeBG, tolerance)
