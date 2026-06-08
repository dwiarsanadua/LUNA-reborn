--[[
  Quest 424: [Quest] It's not enough yet.
  Level Required: 81
  NPC Start: 98, NPC Complete: 98
  Prerequisites: [423]
  Rewards: EXP=2266495, Gold=100000
]]

local fsm = require('fsm_engine')

local quest_424 = fsm:new({
    id = 424,
    name = "[Quest] It's not enough yet.",
    level_required = 81,
    npc_start = 98,
    npc_complete = 98,
    prerequisites = {423},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 424: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 424: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 424: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 276,
                    count = 80,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 424: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 424: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 424: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 424: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 424: Rewards given")
                self:give_rewards({
                    exp = 2266495,
                    gold = 100000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_424