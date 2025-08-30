import itertools
import csv
import os

# the elements for combination, defined in the script
elements = ['-','FIRE', 'WATER', 'AIR', 'ELECTRO', 'EARTH']

# generate all 3-element combinations with replacement
combinations = list(itertools.combinations_with_replacement(elements, 3))

# output to a csv file
script_dir = os.path.dirname(os.path.abspath(__file__))
output_file = script_dir+'\element_combinations.csv'

with open(output_file, 'w', newline='') as f:
    writer = csv.writer(f)
    # write header
    writer.writerow(['Slot 1', 'Slot 2', 'Slot 3'])
    # write each combination as a row
    writer.writerows(combinations)

print(f"Combinations generated and saved to {output_file}")