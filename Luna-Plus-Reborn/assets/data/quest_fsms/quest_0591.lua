--[[
  Quest 591: [Quest] Crying Fungus
  Level Required: 0
  NPC Start: 301, NPC Complete: 301
  Prerequisites: [590]
]]

local fsm = require('fsm_engine')

local quest_591 = fsm:new({
    id = 591,
    name = "[Quest] Crying Fungus",
    level_required = 0,
    npc_start = 301,
    npc_complete = 301,
    prerequisites = {590},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 591: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 301,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 591: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 591: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 21,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 591: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 301,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 591: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 591: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 301,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 591: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 591: Rewards given")
                self:give_rewards({
                    items = {
                        { item_id = 21001100, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_591