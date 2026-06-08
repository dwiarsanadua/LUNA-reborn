--[[
  Quest 2008: [Daily] Piohri Subjugation
  Level Required: 56
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=147052
]]

local fsm = require('fsm_engine')

local quest_2008 = fsm:new({
    id = 2008,
    name = "[Daily] Piohri Subjugation",
    level_required = 56,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2008: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2008: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2008: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 148,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2008: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2008: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2008: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2008: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2008: Rewards given")
                self:give_rewards({
                    exp = 147052,
                    items = {
                        { item_id = 30000704, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2008