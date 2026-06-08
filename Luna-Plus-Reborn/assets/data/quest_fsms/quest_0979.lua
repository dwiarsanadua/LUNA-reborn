--[[
  Quest 979: [Dungeon Quest]Temple of Greed(Beginner)
  Level Required: 90
  NPC Start: 64, NPC Complete: 64
  Rewards: EXP=5222395, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_979 = fsm:new({
    id = 979,
    name = "[Dungeon Quest]Temple of Greed(Beginner)",
    level_required = 90,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 979: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 979: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 979: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 645,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 979: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 654,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 979: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 655,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 979: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 656,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 979: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 979: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 979: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 979: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 979: Rewards given")
                self:give_rewards({
                    exp = 5222395,
                    gold = 61188,
                })
            end,
            transitions = {},
        },
    },
})

return quest_979