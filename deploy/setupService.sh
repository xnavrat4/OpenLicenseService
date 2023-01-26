echo -e "[INFO]\tInstalation started"

sudo cp olsservice.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable olsservice.service
sudo systemctl start olsservice.service
if [ $(systemctl is-active olsservice.service) == "active" ]; then
    echo -e "[INFO]\tService is active!"
fi    

echo -e "[INFO]\tInstalation completed!"
exit
