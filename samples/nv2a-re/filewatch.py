from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
import watchdog.events

registered_files = []

def register_handler(path, callback):
  global registered_files
  handler = 0
  registered_files += [(path, callback)]
  return handler

def unregister_handler(handler):
  global registered_files
  registered_files[handler] = None

def handle_modification(path):
  global registered_files
  print("modification in %s" % path)
  for registered_file in registered_files:
    print("  checking %s" % registered_file)

class MyHandler(watchdog.events.FileSystemEventHandler):
  def on_any_event(self, event):
    print(f'event type: {event.event_type}  path : {event.src_path}')
    handle_modification(event.src_path)
    if isinstance(event, watchdog.events.FileMovedEvent):
      handle_modification(event.dest_path)
