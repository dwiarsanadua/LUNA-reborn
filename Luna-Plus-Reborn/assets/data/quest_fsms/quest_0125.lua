--[[
  Quest 125: Soul Harvest
  Level Required: 1
  NPC Start: 27, NPC Complete: 27
  Prerequisites: [124]
  Rewards: EXP=22224, Gold=5070
]]

local fsm = require('fsm_engine')

local quest_125 = fsm:new({
    id = 125,
    name = "Soul Harvest",
    level_required = 1,
    npc_start = 27,
    npc_complete = 27,
    prerequisites = {124},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 125: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 125: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 125: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 10,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 125: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 26,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 125: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 55,
                    count = 3,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 125: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 125: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 125: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 27,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 125: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 125: Rewards given")
                self:give_rewards({
                    exp = 22224,
                    gold = 5070,
                    items = {
                        { item_id = 64, count = 5 },
                        { item_id = 21000006, count = 5 },
                        { item_id = 65, count = 5 },
                        { item_id = 66, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_125