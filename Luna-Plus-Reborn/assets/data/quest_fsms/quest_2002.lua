--[[
  Quest 2002: [Daily] Basilisk Subjugation
  Level Required: 44
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=68450
]]

local fsm = require('fsm_engine')

local quest_2002 = fsm:new({
    id = 2002,
    name = "[Daily] Basilisk Subjugation",
    level_required = 44,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2002: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2002: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2002: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 38,
                    count = 40,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2002: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2002: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2002: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2002: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2002: Rewards given")
                self:give_rewards({
                    exp = 68450,
                    items = {
                        { item_id = 30000704, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2002