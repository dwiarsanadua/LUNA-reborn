--[[
  Quest 563: Quest 563
  Level Required: 1
  NPC Start: 11, NPC Complete: 11
  Prerequisites: [562]
]]

local fsm = require('fsm_engine')

local quest_563 = fsm:new({
    id = 563,
    name = "Quest 563",
    level_required = 1,
    npc_start = 11,
    npc_complete = 11,
    prerequisites = {562},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 563: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 563: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 563: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 392,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 563: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 186,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 563: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 246,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 563: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 247,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 563: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 248,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 563: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 563: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 563: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 563: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 563: Rewards given")
                self:give_rewards({
                })
            end,
            transitions = {},
        },
    },
})

return quest_563