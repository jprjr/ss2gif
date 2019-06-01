local iup = require'iuplua'

local function main()
  local err

  local output_txt = iup.text({expand = "HORIZONTAL"})
  local folder_txt = iup.text({expand = "HORIZONTAL"})
  local delay_txt = iup.text({expand = "HORIZONTAL",
    spin = "YES",
    spinmin=1,
    spinvalue=2,
  })

  local dlg = iup.dialog({
    iup.vbox({
      iup.gridbox({
        iup.button({
          title = "Choose Folder of images",
          action = function(self)
            local folderdlg = iup.filedlg({
              dialogtype = "DIR",
            })
            folderdlg:popup(iup.ANYWHERE,iup.ANYWHERE)
            folder_txt.value = folderdlg.value
          end,
        }),
        folder_txt,
        iup.button({
          title = "Choose Output Gif",
          action = function(self)
            local filedlg = iup.filedlg({
              dialogtype = "SAVE",
              file = "output.gif",
            })
            filedlg:popup(iup.ANYWHERE,iup.ANYWHERE)
            output_txt.value = filedlg.value
          end
        }),
        output_txt,
        iup.label({ title = "Delay"}),
        delay_txt,
        orientation = HORIZONTAL,
        NUMDIV=2,
        gaplin=20,
        gapcol=20,
      }),
      iup.button({
        title = "Do it!",
        action = function(self)
          gifwrapper(delay_txt.value,output_txt.value,folder_txt.value)
        end,
      }),
    }),
    title = "SS2GIF",
    size="300x100",
  })

  dlg:showxy(iup.CENTER,iup.CENTER)

  if (iup.MainLoopLevel()==0) then
    iup.MainLoop()
    iup.Close()
  end
end

main()
