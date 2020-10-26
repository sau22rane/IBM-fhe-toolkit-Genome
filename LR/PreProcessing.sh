
echo "==========================================================="
echo "Getting User data ready...!!!"
echo "Preprocessing user data.....!!!"
python3 PreProcessing.py
echo "Creating data.txt file.....!!!"
echo "==========================================================="
echo ""
echo "==========================================================="
echo "Getting ID of the current Docker....!!!"
containerID=$(sudo docker ps -l -q)
echo "Container ID : $containerID"
echo "==========================================================="
echo ""
echo "==========================================================="
echo "Uploading data to docker...!!!"
sudo docker cp ./data.txt $containerID:/opt/IBM/FHE-Workspace/examples/Genome/
echo "Data uploaded successfully...!!!"
echo "==========================================================="
