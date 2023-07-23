
$(ROGUEPORYSCRIPTSDIR)/Strings/Rogue_Strings_Items.inc: %.inc: %.pory
	$(PORYSCRIPT) -i $< -o $@ $(PORYSCRIPTARGS) -f 1_latin_rse_items

