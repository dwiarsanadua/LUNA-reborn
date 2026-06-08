--[[
  Quest 943: [Party] John's Story
  Level Required: 0
  NPC Start: 541, NPC Complete: 541
  Prerequisites: [937]
  Rewards: EXP=13488379, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_943 = fsm:new({
    id = 943,
    name = "[Party] John's Story",
    level_required = 0,
    npc_start = 541,
    npc_complete = 541,
    prerequisites = {937},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 943: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 943: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 943: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 327,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 943: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 943: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 943: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 943: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 943: Rewards given")
                self:give_rewards({
                    exp = 13488379,
                    gold = 101632,
                })
            end,
            transitions = {},
        },
    },
})

return quest_943