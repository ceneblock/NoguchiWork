"""This program identifies cells whose bounding boxes overlap and calculates the overlap between the boxes
It then considers objects two different ones if the area of the rectangular overlap is less than 0.2 (20%) of either
bounding box. This needs the CSV combined file after all the individual CSV files have been joined together.
"""

import os
import pandas as pd
import random
import PIL
import re
from PIL import Image
from PIL import ImageDraw
import math
import easygui

#Image.MAX_IMAGE_PIXELS = None
#locat = "I:\\CI30_534\\CI30 534 AC3 10x_28-33_Junk1.csv"
#locat = "D:\\LabSync Files\\Nicole\\Monkey_AI_Labeled\\CI30_534\\CSV\\CI30 534 AC3 10x_28-33_NoDups.csv"

direct = easygui.diropenbox('Where are the files?')
direct = os.path.join(direct)
print(direct)

files = os.listdir(direct)

for images in files:
    if not images.endswith(".csv"):
        continue
    elif images.startswith("._"):  # mac files have these meta data files
        continue
    else:
        DASH = images.count('-')
        if DASH == 2:
            wide1 = re.search('-(.*).csv', images)  # This doesn't capture the second number because there are two -
            wide = re.search('-(.*)AllCSV', wide1.group(1))
        else:
            # Use below if 1 dash in file name
            wide = re.search('-(.*)_AllCSV.csv', images)  # This doesn't capture the second number because there are two -
            # wide = re.search('-(.*).c', wide1.group(1))

        tall = re.search('LSG_(.*)-', images)
        # wide = re.search('-(.*).jpg', images)

        colum1 = wide.group(1)  # if 0 than width = 1350
        row1 = tall.group(1)  # if 0 than tall = 1050
        le = (len(str(colum1)) + len(str(row1)) + 12) * -1
        # le tells us how to isolate the base name
        nam1 = images[:le]
        # print('row: '+row)
        # print('column: ' + colum)
        # print(nam1)

        locat = os.path.join(direct, images)
        #locat = images #I call this images even though it's AllCSV.csv files

        PlotFile = []
        df1 = []
        dele = []
        Over = []
        Frames = []
        Remove = []
        Deleter = []
        DELETER = ""
        NODUPS = []
        df1 = []
        Filler = []


        x = 0
        MinDist = 100

        df1 = pd.read_csv(locat)
        Overlap = []
        Topper = []
        dele = []

        Topper = pd.DataFrame.from_dict(Topper)

        SHAP = (df1.shape)
        r = SHAP[0]  # rows
        c = SHAP[1]  # columns
        # Number of rows and columns
        # print(r,c)

        print(r)

        colorlist = ["red", "green", "orange", "blue", "yellow"]

        for x in range(0, r):  # (0,r) for x in range(0,5350):
            unnam = (df1.loc[x, 'Object'])
            dotx = (df1.loc[x, 'Dotx'])
            doty = (df1.loc[x, 'Doty'])
            scor = (df1.loc[x, 'Score'])
            clas = (df1.loc[x, 'Class'])
            left = (df1.loc[x, 'Left'])
            right = (df1.loc[x, 'Right'])
            top = (df1.loc[x, 'Top'])
            bottom = (df1.loc[x, 'Bottom'])
            area = (right - left) * (bottom - top)  # area of rectangle

            for y in range(x + 1, r):  # (x+1,r)for y in range(x+1, r):
                unnam2 = (df1.loc[y, 'Object'])
                dotx1 = (df1.loc[y, 'Dotx'])
                doty1 = (df1.loc[y, 'Doty'])
                scor1 = (df1.loc[y, 'Score'])
                clas1 = (df1.loc[y, 'Class'])
                left1 = (df1.loc[y, 'Left'])
                right1 = (df1.loc[y, 'Right'])
                top1 = (df1.loc[y, 'Top'])
                bottom1 = (df1.loc[y, 'Bottom'])
                area1 = (right1 - left1) * (bottom1 - top1)  # area of rectangle
                dist = math.sqrt((dotx - dotx1) ** 2 + (doty - doty1) ** 2)

                # If true then rectangles overlap somewhere
                if (((left > left1 and left < right1) or (right > left1 and right < right1) or (
                        left < left1 and right > right1) or (left1 < left and right1 > right)) and (
                        (top > top1 and top < bottom1) or (bottom > top1 and bottom < bottom1) or (
                        top < top1 and bottom > bottom1) or (top1 < top and bottom1 > bottom))):
                    print("Yes**************************************")

                    # Determine the left side of overlapped rectangle
                    if (left > left1 and left < right1):
                        left3 = left
                    else:
                        left3 = left1

                    # Determine the right side of overlapped rectangle
                    if (right > left1 and right < right1):
                        right3 = right
                    else:
                        right3 = right1

                    # Determine the top side of overlapped rectangle
                    if (top > top1 and top < bottom1):
                        top3 = top
                    else:
                        top3 = top1

                    # Determine the bottom side of overlapped rectangle
                    if (bottom > top1 and bottom < bottom1):
                        bottom3 = bottom
                    else:
                        bottom3 = bottom1

                    # examination found that if ratio or ratio1 is above 0.2 it is highly likely
                    # they're 2 different objects
                    area3 = (right3 - left3) * (bottom3 - top3)  # area of overlap
                    print("X: " + str(unnam) + "  Y: " + str(unnam2) + "  Overlap: " + str(area3))
                    ratio = area3 / area  # ratio of overlapped rectangle to first rectangle
                    ratio1 = area3 / area1  # ratio of overlapped rectangle to second rectangle

                    print("area1:" + str(area))
                    print("area2:" + str(area1))
                    print("area3:" + str(area3))

                    if (ratio > 0.2 or ratio1 > 0.2):
                        print("OK**************************************")
                        Overlap.append(
                            [unnam, unnam2, dist, scor, clas, dotx, doty, left, right, top, bottom, left3, right3, top3,
                             bottom3, area, area3])
                        Overlap.append(
                            [unnam2, unnam, dist, scor1, clas1, dotx1, doty1, left1, right1, top1, bottom1, left3,
                             right3, top3, bottom3, area1, area3])
                        if clas < clas1:
                            # df1.drop(labels=x, axis=0, inplace=True)
                            dele.append([unnam])
                            print('deleted' + str(unnam))

                        else:
                            # df1.drop(labels=y, axis=0, inplace=True)
                            dele.append([unnam2])
                            print('deleted' + str(unnam2))

            Over = pd.DataFrame.from_dict(Overlap)
            print("Obj1=" + str(unnam) + "   Obj2=" + str(unnam2))

            Frames = [Over, Topper]
            Topper = pd.concat(Frames)
            # print(Top3)

            Over = []
            Overlap = []


        # In order to delete duplicates I need to convert it to a pandas dataframe
        Deleter = pd.DataFrame.from_dict(dele)

        # This deletes the duplicate objects
        Deleter.drop_duplicates(inplace=True)

        # This saves a file of deleted objects
        DELETER = nam1 + row1 + "-" + colum1
        DELETER = DELETER + "_Deleted.csv"
        DELETER = os.path.join(direct, DELETER)
        print(DELETER)
        Deleter.to_csv(DELETER)

        # List converts back to list
        dele = Deleter.values.tolist()

        # Determines the row and columns of Deleter
        r1 = len(dele)

        # Turns to a pandas dataframe
        NODUPS = pd.DataFrame.from_dict(df1)

        print(dele)
        # This removes overcounts from the delete list
        for w in range(0, r1):
            del1 = (dele[w])
            print(del1)
            NODUPS.drop(del1, inplace=True)
            # print("dropped: " + str(del1)

        # This deletes exact duplicates
        NODUPS = NODUPS.drop_duplicates(subset=['Dotx', 'Doty'], keep="first")

        FILLER = nam1 + row1 + "-" + colum1
        FILLER = os.path.join(direct, FILLER)
        FILLER = FILLER + "_NoDups.csv"

        NODUPS.to_csv(FILLER, index=False)

