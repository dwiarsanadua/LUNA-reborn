--[[
  Quest 2031: [Daily] Centaurs Stomping
  Level Required: 81
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=1133247
]]

local fsm = require('fsm_engine')

local quest_2031 = fsm:new({
    id = 2031,
    name = "[Daily] Centaurs Stomping",
    level_required = 81,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2031: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2031: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2031: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 274,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2031: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2031: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2031: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2031: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2031: Rewards given")
                self:give_rewards({
                    exp = 1133247,
                    items = {
                        { item_id = 30000835, count = 3 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2031