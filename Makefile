all:
	python build.py
	python buildhat.py

dot3k:
	python build.py

dothat:
	python buildhat.py

clean:
	python build.py clean
	python buildhat.py clean
