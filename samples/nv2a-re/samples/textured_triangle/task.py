class Task(BaseTask):
  def __init__(self):

    ConvertImage("tex0.png", texture_A8R8G8B8, "tmp/tex0.bin")
    ConvertImage("tex1.png", texture_A8R8G8B8, "tmp/tex1.bin")
    ConvertImage("tex2.png", texture_A8R8G8B8, "tmp/tex2.bin")
    ConvertImage("tex3.png", texture_A8R8G8B8, "tmp/tex3.bin")

    tex0_bin = ContiguousResourceFromFile("tmp/tex0.bin")
    tex1_bin = ContiguousResourceFromFile("tmp/tex1.bin")
    tex2_bin = ContiguousResourceFromFile("tmp/tex2.bin")
    tex3_bin = ContiguousResourceFromFile("tmp/tex3.bin")

    CompileFP20("fp.fp", "tmp/fp.inl")

    make_pb = BuildC("pb.c", "tmp/pb")
    run_pb = Run("tmp/pb", "tmp/pb.bin", tex0_bin, tex1_bin, tex2_bin, tex3_bin)

    pb_bin = ContiguousResourceFromFile("tmp/pb.bin")
    RunPushbuffer(pb_bin)

task = Task()
task.begin()
while True:
  print("Frame")
  task.force_update()
  time.sleep(1.0)
task.end()
