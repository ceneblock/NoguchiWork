import numpy as np
import threading
import csv

IOU_THRESHOLD = 20
"""
  struct coordinates
  {
    long x1,y1,x2,y2;
    long score;
    int class;
  };
"""

def convert_to_coordinates(input_array, output_vector, position):
    """
    Takes an input array from the csv and converts it into a vector. This can be
    done massively in parallel
    """
    output_vector[position] = np.array([input_array['Left'], input_array['Right'],
            input_array['Top'], input_array['Bottom'], input_array['Score'],
            input_array['Class']])
    
def IoU(coordinates1, coordinates2):

    """
    with lock:
        print("Coordinates:")
        print(coordinates1)
        print("\n")
        print(coordinates2)
    """

    result = 0;
    #Simple code:
    #-1 delete coordinates1
    # 0 delete neither
    # 1 delete coordinates2

    #temp = np.zeros(coordinates1.size)
    temp = []

    temp.insert(0, max(coordinates1[0], coordinates2[0]))
    temp.insert(1, max(coordinates1[1], coordinates2[1]))

    temp.insert(2, min(coordinates1[2], coordinates2[2]))
    temp.insert(3, min(coordinates1[3], coordinates2[3]))

    if(temp[2] <= temp[0] and temp[3] <= temp[1]):
        return result

    intersection = (temp[2] - temp[0]) * (temp[3] - temp[1])

    coordinates1_area = (coordinates1[2] - coordinates1[0]) * \
        (coordinates1[3] - coordinates1[1])

    coordinates2_area = (coordinates2[2] - coordinates2[0]) * \
        (coordinates2[3] - coordinates2[1])
    
    intersection_over_union = intersection / float((coordinates1_area +
        coordinates2_area - intersection))

    if((intersection_over_union * 100) >= IOU_THRESHOLD):
        """
        with lock:
            print("Coordinates1: " + str(coordinates1[4]) + " Coordinates2: " +
                    str(coordinates2[4]))
        """
        if(coordinates1[4] > coordinates2[4]):
            result = 1
        if(coordinates1[4] < coordinates2[4]):
            result = -1
    """
    with lock:
        print("Returning: " + str(result))
    """

    return result

def reduce(in_array, indicies, starting_position):
    temp_array = np.copy(in_array)
    x = 0
    while x < temp_array.shape[0]:
        y = x + 1
        while y < temp_array.shape[0]:
            """
            with lock:
                print("Starting Position: " + str(starting_position))
                print("Len: " + str(len(out_array)))
                print("Y Out array: ")
                print(out_array)
                print("Y in array: ")
                print(in_array)
                print("\n")
            """
            result = IoU(temp_array[x], temp_array[y])
            #There has to be a better way to do this without this parallel array
            #stuff...
            if result == -1:
                #Need to delete ourselves and pop out
                indicies.append(x + starting_position)
                """
                with lock:
                    print("Going to delete: ")
                    print(temp_array[x])
                """
                temp_array = np.delete(temp_array, x, axis=0)
            elif result == 1:
                #The one on the right needs to be deleted
                """
                with lock:
                    print("Going to delete: ")
                    print(temp_array[y])
                """
                indicies.append(y + starting_position)
                temp_array = np.delete(temp_array, y, axis=0)
            y = y + 1
        x = x + 1
    """
    with lock:
        print("Going to return: ")
        print(out_array)
    """

my_data = np.genfromtxt("KN564_NeuN_1-10_LevUSlide1_5_LSG_49-55_AllCSV.csv",
        delimiter=',', dtype=None, names=True, encoding="UTF-8", autostrip=True)

np.sort(my_data, order='File')

print(my_data['File'].size)
unique_files = np.unique(my_data['File'])
print(unique_files.size)

#my_data[:] = my_data[:, [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0]] 
#for element in my_data:
#    print(type(element))
#    for name in element:
#        print("Type: %s Name: %s " % (type(name),  name))
#    print("\n-------------\n")
#
last_row = my_data[0]
counter = 0
different_files = list()
for row in my_data:
    if last_row['File'] != row['File']:
        #print("Different File at " + str(counter) + " old: " + last_row['File']
        #        + " new: " + row['File'])
        different_files.append(counter)
        last_row = row
    counter = counter + 1

output_vector = np.zeros((my_data['File'].size, 6))
for x in range(my_data['File'].size):
    threading.Thread(target=convert_to_coordinates, args=(my_data[x],
        output_vector,x)).start()
"""
print("Output Vector")
print(output_vector)
in_array = np.array([[0,0, 5,5, .50, 1],
    [1,1,6,6,.80,2],[3,3,8,8,.8,3],[4,4,9,9,.50,5]])
#lock = threading.Lock()
"""
out_array = np.copy(my_data).tolist()
"""
print("In Array: " )
print(in_array)

print("\n")

print("Out Array: ")
print(out_array)

print("\n")
"""
t = []
indicies = []
x = 0
while x < len(different_files):
    t.append(threading.Thread(target=reduce, 
        args=(my_data[x:different_files[x+1]], indicies, x)))
    t[-1].start()
    x = x + 1

x = 0
while x < len(different_files):
    t[x].join()
    x = x + 1
"""
print("In Array: " )
print(in_array)

print("\n")
"""
indicies.sort(reverse = True)
for x in indicies:
    del out_array[x]

#print("Final Out Array: ")
#print(out_array)
with open('numpy_thread.csv', 'w') as f:
      
    # using csv.writer method from CSV package
    write = csv.writer(f)
      
    #write.writerow(fields)
    write.writerows(out_array)
"""
print("indicies: ")
print(indicies)
print("\n")
"""
