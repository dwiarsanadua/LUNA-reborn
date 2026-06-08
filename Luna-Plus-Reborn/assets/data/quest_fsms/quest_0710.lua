--[[
  Quest 710: Excellent Adhesive
  Level Required: 65
  NPC Start: 45, NPC Complete: 45
  Prerequisites: [706]
  Rewards: EXP=987718
]]

local fsm = require('fsm_engine')

local quest_710 = fsm:new({
    id = 710,
    name = "Excellent Adhesive",
    level_required = 65,
    npc_start = 45,
    npc_complete = 45,
    prerequisites = {706},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 710: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 710: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 710: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 64,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 710: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 710: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 710: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 45,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 710: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 710: Rewards given")
                self:give_rewards({
                    exp = 987718,
                    items = {
                        { item_id = 355, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_710