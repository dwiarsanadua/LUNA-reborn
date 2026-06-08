--[[
  Quest 2003: [Daily] Procurement 1
  Level Required: 46
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=99789
]]

local fsm = require('fsm_engine')

local quest_2003 = fsm:new({
    id = 2003,
    name = "[Daily] Procurement 1",
    level_required = 46,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2003: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2003: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2003: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2003: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2003: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2003: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2003: Rewards given")
                self:give_rewards({
                    exp = 99789,
                    items = {
                        { item_id = 30000704, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2003