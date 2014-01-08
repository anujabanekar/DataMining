import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.ObjectInputStream.GetField;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;
import java.util.TreeMap;

import au.com.bytecode.opencsv.CSVReader;

public class Cluster {

	static boolean isLong(String s) {
		try {
			Long i = Long.parseLong(s);
			return true;
		}

		catch (NumberFormatException er) {
			return false;
		}
	}

	public static void readLinesFromFile(String fileFullPathName, String nc) {
		CSVReader reader = null;
		DataOutputStream out = null;

		try {

			reader = new CSVReader(new FileReader(fileFullPathName), ',');
			// FileInputStream fstream = new FileInputStream(fileFullPathName);
			// DataInputStream in = new DataInputStream(fstream);
			// BufferedReader br = new BufferedReader(new
			// InputStreamReader(in));
			int cc = Integer.parseInt(nc);

			Scanner scan = new Scanner(new File("sort.txt"));
			Map<String, String> idf_values_token = new HashMap<String, String>();

			OutputStream outputStream = new FileOutputStream(
					"clustervalues.txt");
			out = new DataOutputStream(outputStream);

			int j = 0;

			while (scan.hasNext() && j < cc) {
				String line = scan.nextLine();
				int idx = line.indexOf(' ');
				String value = line.substring(0, idx);
				String token = line.substring(idx).trim();
				idf_values_token.put(token, value);
				out.writeBytes(token + "\n");
				j++;
			}

			// out1.writeBytes("\nIds,Clusterno");
			String[] nextLine;
			// Read one line at a time
			long row = 0;

			BufferedWriter bw = new BufferedWriter(new FileWriter("result.txt"));
			bw.append("QuestionId,ClusterId\n");

			while ((nextLine = reader.readNext()) != null) {

				int column = 0;
				for (String token : nextLine) {
					// Print all tokens
					column++;
					if (column == 1) {

						if (isLong(token)) {
							row = Long.parseLong(token);
						}
					}

					if (column == 2 || column == 3) {

						BufferedReader br = new BufferedReader(new FileReader(
								"clustervalues.txt"));
						// System.out.println("Reading");
						// System.out.println(row + "--" + column + "--" +
						// token);

						String searchList;
						int clusterNo = 0;
						while ((searchList = br.readLine()) != null) {
							// System.out.println("Cluster : " + searchList);
							clusterNo++;
							String searchKey[] = searchList.split("-");

							for (String key : searchKey) {
								if (token.contains(key)) {
									bw.append(row + "," + clusterNo + "\r\n");
									break;
								}
							}

						}

						//System.out.println("Reading Closed");
						br.close();

					}

				}

				/*
				 * if(row==30) {
				 * 
				 * break; }
				 */
				// row++;
			}

			// line.split(cvsSplitquotes);
			// line.split(cvsSplitquotes);
			// line.split(cvsSplitBy);
			// line = br.readLine();

			System.out.println("Writing Closed");
			bw.close();
			scan.close();
			out.close();

		} catch (Exception e) {
			System.err.println("Error: " + e.getMessage());
			System.exit(0);
		} finally {
			try {
				reader.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}

	}

	public static void main(String args[]) throws IOException {
		Scanner console = new Scanner(System.in);

		String fileName;
		String noofclusters;

		BufferedReader reader = null;
		PrintWriter outputStream = null;
		ArrayList<String> rows = new ArrayList<String>();

		try {
			reader = new BufferedReader(new FileReader("vector_file.txt"));
			outputStream = new PrintWriter(new FileWriter("sort.txt"));

			String file;
			while ((file = reader.readLine()) != null) {
				rows.add(file);
			}
			// Collections.sort(rows);
			Collections.sort(rows, Collections.reverseOrder());
			String[] strArr = rows.toArray(new String[0]);
			for (String cur : strArr)
				outputStream.println(cur);
			// System.out.println("Done");
		} finally {
			if (reader != null) {
				reader.close();
			}
			if (outputStream != null) {
				outputStream.close();
			}
		}

		System.out.printf("Enter the filename:\t");

		fileName = console.nextLine();

		System.out.printf("Enter number of clusters:\t");

		noofclusters = console.nextLine();
		// int noofclusters = Integer.parseInt(args[1]);
		readLinesFromFile(fileName, noofclusters);

	}

}
