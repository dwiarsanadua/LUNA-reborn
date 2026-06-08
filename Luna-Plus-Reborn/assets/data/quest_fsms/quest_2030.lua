--[[
  Quest 2030: [Daily] Is it Pegasuses or Pegasusii?
  Level Required: 80
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=1176541
]]

local fsm = require('fsm_engine')

local quest_2030 = fsm:new({
    id = 2030,
    name = "[Daily] Is it Pegasuses or Pegasusii?",
    level_required = 80,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2030: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2030: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2030: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 271,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2030: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2030: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2030: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2030: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2030: Rewards given")
                self:give_rewards({
                    exp = 1176541,
                    items = {
                        { item_id = 30000835, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2030