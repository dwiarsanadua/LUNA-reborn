--[[
  Quest 549: [Repeat][Quest] Magical Horns
  Level Required: 150
  NPC Start: 98, NPC Complete: 98
  Prerequisites: [548]
  Rewards: EXP=1000000
]]

local fsm = require('fsm_engine')

local quest_549 = fsm:new({
    id = 549,
    name = "[Repeat][Quest] Magical Horns",
    level_required = 150,
    npc_start = 98,
    npc_complete = 98,
    prerequisites = {548},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 549: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 549: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 549: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 271,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 549: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 549: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 549: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 549: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 549: Rewards given")
                self:give_rewards({
                    exp = 1000000,
                    items = {
                        { item_id = 345, count = 1 },
                        { item_id = 30000039, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_549