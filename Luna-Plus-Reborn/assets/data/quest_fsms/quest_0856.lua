--[[
  Quest 856: Magapui Tears 2
  Level Required: 89
  NPC Start: 128, NPC Complete: 128
  Prerequisites: [855]
  Rewards: EXP=4664309
]]

local fsm = require('fsm_engine')

local quest_856 = fsm:new({
    id = 856,
    name = "Magapui Tears 2",
    level_required = 89,
    npc_start = 128,
    npc_complete = 128,
    prerequisites = {855},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 856: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 856: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 856: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 283,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 856: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 856: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 856: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 856: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 856: Rewards given")
                self:give_rewards({
                    exp = 4664309,
                    items = {
                        { item_id = 396, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_856