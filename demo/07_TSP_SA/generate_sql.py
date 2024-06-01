import csv
import sys

def generate_sql(csv_file, sql_file):
    with open(csv_file, newline='') as csvfile:
        reader = csv.reader(csvfile, delimiter=';')
        with open(sql_file, 'w') as sqlfile:
            sqlfile.write("USE projects;\n")
            for row in reader:
                instance = row[0]
                initial_evaluation = row[1]
                best_evaluation = row[2]
                improvement = row[3]
                execution_time = row[4]
                best_solution = row[5]
                sql = f"INSERT INTO tsp_brkga_data (instance, initial_evaluation, best_evaluation, improvement, execution_time, best_solution, source) VALUES ('{instance}', {initial_evaluation}, {best_evaluation}, {improvement}, {execution_time}, '{best_solution}', 'Python');\n"
                sqlfile.write(sql)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python generate_sql.py input.csv output.sql")
    else:
        csv_file = sys.argv[1]
        sql_file = sys.argv[2]
        generate_sql(csv_file, sql_file)
