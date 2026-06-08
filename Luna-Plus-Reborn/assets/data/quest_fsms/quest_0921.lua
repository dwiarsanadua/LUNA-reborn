--[[
  Quest 921: [Party][Quest] The Last Bluebeard
  Level Required: 100
  NPC Start: 538, NPC Complete: 538
  Rewards: EXP=11363827, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_921 = fsm:new({
    id = 921,
    name = "[Party][Quest] The Last Bluebeard",
    level_required = 100,
    npc_start = 538,
    npc_complete = 538,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 921: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 921: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 921: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 294,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 921: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 295,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 921: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 921: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 921: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 921: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 921: Rewards given")
                self:give_rewards({
                    exp = 11363827,
                    gold = 81769,
                })
            end,
            transitions = {},
        },
    },
})

return quest_921