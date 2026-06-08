--[[
  Quest 430: [Party][Quest]A Haunt of Robbers
  Level Required: 84
  NPC Start: 99, NPC Complete: 99
  Prerequisites: [429]
  Rewards: EXP=2964862
]]

local fsm = require('fsm_engine')

local quest_430 = fsm:new({
    id = 430,
    name = "[Party][Quest]A Haunt of Robbers",
    level_required = 84,
    npc_start = 99,
    npc_complete = 99,
    prerequisites = {429},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 430: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 430: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 430: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 279,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 430: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 430: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 430: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 99,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 430: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 430: Rewards given")
                self:give_rewards({
                    exp = 2964862,
                    items = {
                        { item_id = 30000232, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_430