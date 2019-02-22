using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Linq;

namespace PeMultiboot
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 2)
            {
                Console.WriteLine("Usage: PeMultiboot <load addr> <PE file>");
            }
            try
            {
                Console.WriteLine("Adding Multiboot header to: " + args[1] + ", load addr: " + args[0]);
                if (!args[0].StartsWith("0x"))
                {
                    Console.WriteLine("Specify load address as hex number, ex: 0x100000");
                    Environment.Exit(1);
                }
                uint loadAddress = uint.Parse(args[0].Substring(2, args[0].Length - 2), System.Globalization.NumberStyles.HexNumber);

                FileStream file = File.Open(args[1], FileMode.Open, FileAccess.ReadWrite);
                BinaryReader reader = new BinaryReader(file);

                reader.BaseStream.Seek(0x3c, SeekOrigin.Begin);
                uint peHeaderOffset = reader.ReadUInt32();
                reader.BaseStream.Seek(peHeaderOffset, SeekOrigin.Begin);
                if (reader.ReadUInt32() != 0x4550)
                {
                    Console.WriteLine("Can't find PE header.");
                    Environment.Exit(1);
                }

                peHeaderOffset += 4; // PE signature is not part of header
                reader.BaseStream.Seek(peHeaderOffset + 2, SeekOrigin.Begin);
                ushort numberOfSections = reader.ReadUInt16();
                reader.BaseStream.Seek(peHeaderOffset + 16, SeekOrigin.Begin);
                ushort optionalHeaderSize = reader.ReadUInt16();
                if (optionalHeaderSize == 0)
                {
                    Console.WriteLine("Not an image file");
                    Environment.Exit(1);
                }

                uint peOptionalHeaderOffset = peHeaderOffset + 20;

                reader.BaseStream.Seek(peOptionalHeaderOffset, SeekOrigin.Begin);
                if (reader.ReadUInt16() != 0x10B)
                {
                    Console.WriteLine("Not PE32 image.");
                    Environment.Exit(1);
                }

                reader.BaseStream.Seek(peOptionalHeaderOffset + 4, SeekOrigin.Begin);
                uint textSize = reader.ReadUInt32();
                uint dataSize = reader.ReadUInt32();
                uint bssSize = reader.ReadUInt32();
                Console.WriteLine("Code size: {0}", textSize);
                Console.WriteLine("Data size: {0}", dataSize);
                Console.WriteLine("BSS size: {0}", bssSize);

                reader.BaseStream.Seek(peOptionalHeaderOffset + 16, SeekOrigin.Begin);
                uint entryPointAddress = reader.ReadUInt32();
                uint baseOfCode = reader.ReadUInt32();
                Console.WriteLine("Entry: 0x{0:X}", entryPointAddress);
                Console.WriteLine("Base of code: 0x{0:X}", baseOfCode);

                reader.BaseStream.Seek(peOptionalHeaderOffset + 28, SeekOrigin.Begin);
                uint imageBase = reader.ReadUInt32();
                Console.WriteLine("Image base: 0x{0:X}", imageBase);

                uint sectionsOffset = peOptionalHeaderOffset + optionalHeaderSize;
                Console.WriteLine("Sections: " + numberOfSections);

                uint dataEnd = 0;
                uint bssEnd = 0;
                for (int i = 0; i < numberOfSections; i++)
                {
                    reader.BaseStream.Seek(sectionsOffset + i * 40, SeekOrigin.Begin);
                    byte[] data = reader.ReadBytes(8);
                    string name = ParseString(data);
                    Console.WriteLine("Section: " + name);
                    uint virtualSize = reader.ReadUInt32();
                    uint virtualAddress = reader.ReadUInt32();
                    uint sizeOfRawData = reader.ReadUInt32();
                    uint pointerToRawData = reader.ReadUInt32();
                    dataEnd = Math.Max(dataEnd, pointerToRawData + sizeOfRawData);
                    bssEnd = Math.Max(bssEnd, pointerToRawData + virtualSize);
                    Console.WriteLine(" VSize: 0x{0:X}", virtualSize);
                    Console.WriteLine(" VAddr: 0x{0:X}", virtualAddress);
                    Console.WriteLine(" Raw size: 0x{0:X}", sizeOfRawData);
                    Console.WriteLine(" Data at: 0x{0:X}", pointerToRawData);
                }
                bssEnd = Math.Max(bssEnd, dataEnd);

                Console.WriteLine("Load addr: 0x{0:X}", loadAddress);
                Console.WriteLine("Entry: 0x{0:X}", entryPointAddress + loadAddress);
                Console.WriteLine("Data end: 0x{0:X}", dataEnd + loadAddress);
                Console.WriteLine("Bss end: 0x{0:X}", bssEnd + loadAddress);
                WriteMultibootRecord(file, entryPointAddress + loadAddress, loadAddress, dataEnd + loadAddress, bssEnd + loadAddress);
            }
            catch (Exception e)
            {
                Console.WriteLine("Error: " + e.Message);
            }

            Environment.Exit(0);
        }

        private static void WriteMultibootRecord(FileStream file, uint entryPointAddress, uint loadAddress, uint dataEnd, uint bssEnd)
        {
            BinaryWriter writer = new BinaryWriter(file);
            writer.BaseStream.Seek(0, SeekOrigin.Begin);
            uint magic = 0x1BADB002;
            uint flags =
               1 << 0 |
               1 << 1 |
               1 << 16;
            uint checksum = (uint)(-(0x1BADB002 + flags));
            writer.Write(magic);
            writer.Write(flags);
            writer.Write(checksum);
            writer.Write(loadAddress); // header_addr
            writer.Write(loadAddress); // load_addr
            writer.Write(dataEnd); // load_end_addr (data end)
            writer.Write(bssEnd); // bss_end_addr
            writer.Write(entryPointAddress); // entry_addr
        }

        private static string ParseString(byte[] data)
        {
            int i;
            for (i = 0; i < data.Length; ++i)
            {
                if (data[i] == 0)
                {
                    break;
                }
            }
            return UTF8Encoding.UTF8.GetString(data, 0, i);
        }
    }
}