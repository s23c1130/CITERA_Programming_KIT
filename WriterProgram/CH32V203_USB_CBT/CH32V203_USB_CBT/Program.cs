using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.Runtime.Remoting.Channels;
using System.Threading;

namespace CH32V203_USB_CBT
{
    internal class Program
    {
        static private string ReadIniValue(string section, string key, string defaultValue, string filePath)
        {
            try
            {
                // iniファイルをUTF-8で読み込む
                using (StreamReader reader = new StreamReader(filePath, Encoding.UTF8))
                {
                    string line;
                    string currentSection = null;  // 現在処理中のセクション

                    while ((line = reader.ReadLine()) != null)
                    {
                        // 各行を読み込み
                        line = line.Trim();
                        // セクションの始まりを検出
                        if (line.StartsWith("[") && line.EndsWith("]"))
                        {
                            currentSection = line.Substring(1, line.Length - 2).Trim();
                        }
                        else if (currentSection != null && currentSection.Equals(section, StringComparison.OrdinalIgnoreCase))
                        {
                            // 現在処理中のセクションが指定されたセクションと一致する場合
                            if (line.StartsWith($"{key}=", StringComparison.OrdinalIgnoreCase))
                            {
                                // 該当のキーが見つかった場合、値を取得
                                string value = line.Substring(key.Length + 1).Trim();

                                // ダブルクオーテーションで囲まれている場合は取り除く
                                if (!string.IsNullOrEmpty(value) && value.StartsWith("\"") && value.EndsWith("\""))
                                {
                                    value = value.Substring(1, value.Length - 2);
                                }
                                return value;
                            }
                        }
                    }
                }
                // 該当するキーが見つからなかった場合はデフォルト値を返す
                return defaultValue;
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                return defaultValue;
            }
        }

        static void Main(string[] args)
        {

            string iniFilePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "setting.ini");
            string makefileDir = ReadIniValue("SETTING", "makefileDirectory", @"file\obj", iniFilePath);
            string makePath = ReadIniValue("SETTING", "makePath", @"..\..\tools\build Tools\bin\make.exe", iniFilePath);
            string WriterCLIPath = ReadIniValue("SETTING", "WriteCLIPath", @"..\..\tools\wchisp-win-x64\wchisp.exe", iniFilePath);
            string BinaryDataPath = ReadIniValue("SETTING", "BinaryDataPath", @"files\obj\CH32V203K8T6CW.hex", iniFilePath);

            // 上に書くファイル
            string file1 = @"files\main1.c";
            // 真ん中に書くファイル
            string middleFile = "code.ino";
            // 下に書くファイル
            string file2 = @"files\main2.c";

            // 出力ファイル
            string outputFile = @"files\User\main.c";

            using (StreamWriter writer = new StreamWriter(outputFile))
            {
                // 1. ファイル1の内容を上に書く
                if (File.Exists(file1))
                {
                    foreach (string line in File.ReadLines(file1))
                    {
                        writer.WriteLine(line);
                    }
                }

                // 2. 真ん中のファイルの内容を書く
                if (File.Exists(middleFile))
                {
                    foreach (string line in File.ReadLines(middleFile))
                    {
                        writer.WriteLine(line);
                    }
                }

                // 3. ファイル2の内容を下に書く
                if (File.Exists(file2))
                {
                    foreach (string line in File.ReadLines(file2))
                    {
                        writer.WriteLine(line);
                    }
                }
            }

            Console.WriteLine("結合が完了しました: " + outputFile);

            if (File.Exists(middleFile))  // ファイルが存在するか確認
            {
                File.Delete(middleFile);  // ファイルを削除
                Console.WriteLine("ファイルを削除しました。");
            }


            Console.WriteLine("コンパイル開始...");

            Directory.SetCurrentDirectory(makefileDir);
            Console.WriteLine("カレントディレクトリ：" + Directory.GetCurrentDirectory());

            Process CompilerInfo = new Process();
            CompilerInfo.StartInfo.FileName = makePath; //コマンド
            CompilerInfo.StartInfo.Arguments = @"all"; //引数
            CompilerInfo.StartInfo.CreateNoWindow = true; // コンソール・ウィンドウを開かない
            CompilerInfo.StartInfo.UseShellExecute = false; // シェル機能を使用しない
            CompilerInfo.StartInfo.RedirectStandardOutput = true; // 標準出力をリダイレクト
            CompilerInfo.StartInfo.RedirectStandardError = true; //標準エラー出力をリダイレクト

            CompilerInfo.OutputDataReceived += (sender, ev) =>
            {
                Console.WriteLine(ev.Data);
            };
            CompilerInfo.ErrorDataReceived += (sender, ev) =>
            {
                Console.WriteLine(ev.Data);
            };

            //プロセススタート
            CompilerInfo.Start();

            //非同期出力開始
            CompilerInfo.BeginErrorReadLine();
            CompilerInfo.BeginOutputReadLine();

            //終了まで待つ
            CompilerInfo.WaitForExit();

            if (CompilerInfo.ExitCode != 0)
            {
                Console.Write("コンパイル時にエラーが発生しました。エラーコード: ");
                Console.WriteLine(CompilerInfo.ExitCode.ToString());
                Console.WriteLine("終了するにはなにかのキーを押してください...\r\n");
                Console.ReadKey();
                return;
            }

            Console.WriteLine("書き込み開始...");
            Process WCHISP_Proc = new Process();
            WCHISP_Proc.StartInfo.FileName = WriterCLIPath; //コマンド
            WCHISP_Proc.StartInfo.Arguments = @"flash " + BinaryDataPath; //引数
            WCHISP_Proc.StartInfo.CreateNoWindow = true; // コンソール・ウィンドウを開かない
            WCHISP_Proc.StartInfo.UseShellExecute = false; // シェル機能を使用しない
            WCHISP_Proc.StartInfo.RedirectStandardOutput = true; // 標準出力をリダイレクト
            WCHISP_Proc.StartInfo.RedirectStandardError = true; //標準エラー出力をリダイレクト

            WCHISP_Proc.OutputDataReceived += (sender, ev) =>
            {
                Console.WriteLine(ev.Data);
            };
            WCHISP_Proc.ErrorDataReceived += (sender, ev) =>
            {
                Console.WriteLine(ev.Data);
            };

            //プロセススタート
            WCHISP_Proc.Start();

            //非同期出力開始
            WCHISP_Proc.BeginErrorReadLine();
            WCHISP_Proc.BeginOutputReadLine();

            //終了まで待つ
            WCHISP_Proc.WaitForExit();

            if (WCHISP_Proc.ExitCode != 0)
            {
                Console.Write("書き込み時にエラーが発生しました。エラーコード: ");
                Console.WriteLine(WCHISP_Proc.ExitCode.ToString());
                Console.WriteLine("終了するにはなにかのキーを押してください...\r\n");
                Console.ReadKey();
                return;
            }

            Console.WriteLine("書き込みが終了しました。\r\n終了するにはなにかのキーを押してください...");
            Console.ReadKey();


        }
        
    }
}
