--[[
  Quest 453: [Change of Job] Proof of Hero
  Level Required: 105
  NPC Start: 122, NPC Complete: 125
  Prerequisites: [450, 451, 452]
]]

local fsm = require('fsm_engine')

local quest_453 = fsm:new({
    id = 453,
    name = "[Change of Job] Proof of Hero",
    level_required = 105,
    npc_start = 122,
    npc_complete = 125,
    prerequisites = {450, 451, 452},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 453: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 453: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 453: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 453: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 122,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 453: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 453: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 125,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 453: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 453: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 30000544, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_453