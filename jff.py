from random import randint
import os
import re
import requests

def download(url):
    try:
        response = requests.get(url)
        response.raise_for_status()
        return(response.text)
    except requests.exceptions.RequestException as e:
        print(e)

def main(unholyWords):
    holyWords = ["accipicchia", "mele melinda", "rosso di sera", "accidenti", "porca paletta", "diamine", "caspita"]
    
    file = input("Path to un-holy file:\t")
    while not (os.path.isfile(file)):
        print("FIGLIOLO")
        file = input("Path to un-holy file:\t")

    filename, file_extension = os.path.splitext(file)
    with open(file, "r", encoding="utf-8") as source:
        with open(filename + '_godApproved' + file_extension, "w", encoding="utf-8") as target: 
            for line in source:
                holy_line = line
                for bad_word in unholyWords:
                    pattern = re.compile(re.escape(bad_word), re.IGNORECASE)
                    while pattern.search(holy_line):
                        blessing = holyWords[randint(0, len(holyWords)-1)]
                        holy_line = pattern.sub(blessing, holy_line, count=1)
                target.write(holy_line)

if __name__ == "__main__":
    unholyWords = set(download("https://raw.githubusercontent.com/Rattlyy/bestemmiometro/master/bestemmie.txt").splitlines())
    unholyWords.update(download("https://raw.githubusercontent.com/napolux/paroleitaliane/main/paroleitaliane/lista_badwords.txt").splitlines())
    main(unholyWords)
    print("Now God is happy :)")
