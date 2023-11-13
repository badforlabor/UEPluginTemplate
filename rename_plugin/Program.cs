


using System.Text;

/// <summary>
/// 修改插件名字：将src名字替换成dst名字，包括
///         文件夹，文件，文件内容
///     方案：无脑字符串替换
/// 
/// </summary>

class Program
{
    static void Main(string[] args)
    {
        var src = "SceneTools";
        var dst = "XxxPlugin";
        var rootFolder = @"E:\UnrealProjects\test511cpp\Plugins\SceneTools\";
        
        if (args.Length == 3)
        {
            rootFolder = args[0];
            src = args[1];
            dst = args[2];
        }

        rootFolder = Path.GetFullPath(rootFolder);
        rootFolder = rootFolder.TrimEnd('\\');

        // todo。只更改Resource文件夹

        // 改文件内容
        {
            var files = Directory.GetFiles(rootFolder, "*", SearchOption.AllDirectories);
            
            foreach (var file in files)
            {
                var content = File.ReadAllText(file);
                var newContent = content.Replace(src, dst);
                
                // 替换宏定义：MYPLUGINEDITOR_API
                newContent = newContent.Replace(src.ToUpper(), dst.ToUpper());
                
                if (newContent != content)
                {
                    // todo，去掉只读属性
                    File.WriteAllText(file, newContent, Encoding.UTF8);
                }
            }
        }
        
        
        // 改文件夹名字
        {
            var folders = new List<string>(Directory.GetDirectories(rootFolder, "*", SearchOption.AllDirectories));
            folders.Reverse();
            
            foreach (var folder in folders)
            {
                var relFolder = folder.Substring(rootFolder.Length+1);
                var newRelFolder = relFolder.Replace(src, dst);
                
                if (newRelFolder != relFolder)
                {
                    var finalFolder = Path.Combine(rootFolder, newRelFolder);
                    try
                    {
                        Directory.Move(folder, finalFolder);
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e);
                    }
                }
            }
        }

        // 改文件名字
        {
            var files = Directory.GetFiles(rootFolder, "*", SearchOption.AllDirectories);
            
            foreach (var file in files)
            {
                var relFile = file.Substring(rootFolder.Length+1);
                var newRelFile = relFile.Replace(src, dst);
                if (newRelFile != relFile)
                {
                    var finalFile = Path.Combine(rootFolder, newRelFile);
                    File.Move(file, finalFile, true);                    
                }
            }
        }

    }
}