--[[
  Quest 385: [A Class] Smells Like Trouble
  Level Required: 68
  NPC Start: 95, NPC Complete: 95
  Prerequisites: [384]
  Rewards: EXP=994997
]]

local fsm = require('fsm_engine')

local quest_385 = fsm:new({
    id = 385,
    name = "[A Class] Smells Like Trouble",
    level_required = 68,
    npc_start = 95,
    npc_complete = 95,
    prerequisites = {384},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 385: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 385: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 385: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 40,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 385: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 88,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 385: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 89,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 385: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 385: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 385: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 95,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 385: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 385: Rewards given")
                self:give_rewards({
                    exp = 994997,
                })
            end,
            transitions = {},
        },
    },
})

return quest_385