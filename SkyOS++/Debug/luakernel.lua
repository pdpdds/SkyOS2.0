timer_ticks = 0
local tasks = {}

function hline(x1, y, x2, r, g, b)
  if x2 > x1 then
    for i = 0, x2 - x1, 1 do
      putpixel(x1 + i, y, r, g, b)
    end
  end
end

function vline(x, y1, y2, r, g, b)
  if y2 > y1 then
    for i = 0, y2 - y1, 1 do
      putpixel(x, y1 + i, r, g, b)
    end
  end
end

function rectangle(x, y, width, height, r, g, b)
  hline(x, y, x + width, r, g, b)
  hline(x, y + height, x + width, r, g, b)
  vline(x, y, y + height, r, g, b)
  vline(x + width, y, y + height, r, g, b)
end

local function wait(ticks)
  local me, main = coroutine.running()
  for _, task in ipairs(tasks) do
    if task.co == me then
      task.wait_until = timer_ticks + ticks
      break
    end
  end
  coroutine.yield()
end

function taskadd(func, name, maskcount)
  local co = coroutine.create(func)
  setmaskhook(co, maskcount or 1000)
  local task = {
    name = name or "task_" .. #tasks,
    -- For restarting the task if it crashes.
    func = func,
    wait_until = 0,
    co = co,
  }
  table.insert(tasks, task)
  return task
end

-- TODO: don't poll
local function keyboard_task()
  while 1 do
    local scancodes    = get_keyboard_interrupt()
	
	if(scancodes == nil) then
		--print("asddsa")
	else
		--print(scancodes)
		local chunk, errmsg = load(scancodes)
      if chunk == nil then
        print(errmsg)
      else
        local ok, err = pcall(function() chunk() end)
        if not ok then
          print(err)
        end
      end
	end
    --for _, scancode in ipairs(scancodes) do
      --key_pressed(scancode)
    --end
    wait(5)
  end
end

local function blue_rect_task()
  local blue = 20
  while 1 do
    for i = 1, 8 do
      rectangle(DISPLAY_WIDTH - 400, 120, 100, 100, 0, 0, blue)
      wait(5)
    end
    blue = blue + 5
    if blue >= 255 then
      blue = 20
    end
  end
end

local function red_rect_task()
  local red = 20
  while 1 do
    for i = 1, 4 do
      rectangle(DISPLAY_WIDTH - 420, 100, 100, 100, red, 0, 0)
      wait(5)
    end
    red = red + 1
    if red >= 255 then
      red = 20
    end
  end
end

local function green_rect_task()
  local green = 20
  while 1 do
    for i = 1, 6 do
      rectangle(DISPLAY_WIDTH - 410, 110, 100, 100, 0, green, 0)
      wait(5)
    end
    green = green + 3
    if green >= 255 then
      green = 20
    end
  end
end

local function display_task()
  while 1 do
    -- Draw a border around the screen.
    rectangle(0, 0, DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, 255, 255, 255)
    if tilda then
      draw_tilda()
    end
    
    -- Memory usage.
    local bytes_used = math.ceil(collectgarbage("count"))
    local message = ("memory usage: %d kilobytes     "):format(bytes_used)
    --drawtext(DISPLAY_WIDTH - #message * 8, 150, message)
    
    --draw_terminal()
    --swap_buffers()
    wait(10)
  end
end

taskadd(keyboard_task, "keyboard")
taskadd(red_rect_task, "red_rect")
taskadd(green_rect_task, "green_rect")
taskadd(display_task, "display")
taskadd(blue_rect_task, "blue_rect")
--[[
The scheduler task is never preempted because lua_sethook() has not been called on it.
--]]
while 1 do
  sleep()
  timer_ticks = get_timer_ticks()
  local any_tasks_ready = false
  for _, task in ipairs(tasks) do
    local costatus = coroutine.status(task.co)
    if costatus == "suspended" or costatus == "normal" then
      if task.wait_until <= timer_ticks then
        any_tasks_ready = true
        local ok, errmsg = coroutine.resume(task.co)
        if not ok then
          -- print(task.name .. ": " .. errmsg)
          print(errmsg)
          -- Restart the task.
          -- taskadd(task.func, task.name)
        end
      end
    end
  end
  if not any_tasks_ready then
    -- Idle until the next interrupt.
    hlt()
  end
end
